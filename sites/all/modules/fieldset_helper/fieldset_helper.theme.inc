<?php
/**
 * @file
 * Theme functions for fieldset helper module
 */


/**
 * Output toggle all fieldsets link.
 */
function fieldset_helper_toggle_all_output() {
  global $theme;

  if (strpos($_GET['q'], 'admin/build/modules') === 0) {
    $toggle_all_selectors = array(
      'rubik' => '#system-modules div.column-wrapper > fieldset.collapsible',
      'default' => '#system-modules > div > fieldset.collapsible',
    );
    $toggle_all_selector = $toggle_all_selectors[$theme] ? $toggle_all_selectors[$theme] : $toggle_all_selectors['default'];
  }
  else {
    $toggle_all_selector = '';
  }

  return theme('fieldset_helper_toggle_all', $toggle_all_selector);
}


////////////////////////////////////////////////////////////////////////////////
// Theme functions
////////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of hook_theme().
 */
function fieldset_helper_theme() {
  return array(
    'fieldset_helper_toggle_all' => array(
      'arguments' => array('selector' => NULL),
      'template' => 'templates/fieldset-helper-toggle-all',
    ),
  );
}

/**
 * Preprocess variables for the fieldset-helper-toggle-all.tpl.php file.
 */
function template_preprocess_fieldset_helper_toggle_all(&$variables) {
  // Get id
  $id = 'fieldset-helper-toggle-all';
  $id .= (!empty($variables['selector'])) ? preg_replace('/[^-a-z0-9]+/', '-', $variables['selector']) : '';
  $id = form_clean_id($id);

  // Add id to template variables
  $variables['id'] = $id;

  // Add toggle all id and selector to js settings.
  $settings['fieldset_helper_toggle_all'][$id] = $variables['selector'];
  drupal_add_js($settings, 'setting');
}


////////////////////////////////////////////////////////////////////////////////
// Theme preprocess functions
////////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of hook_preprocess_page().
 */
function fieldset_helper_preprocess_page(&$variables) {
  // Make sure the user can save a fieldset's state
  if (!user_access('save fieldset state')) {
    return;
  }
  global $theme;

  // Count the number of collapsible fieldsets.
  $number_of_collapsible_fieldsets = 0;
  $region_list = system_region_list($theme);
  $region_list['content'] = TRUE; // Make sure 'content' is added to the region list.
  $regions = array_keys($region_list);
  foreach ($regions as $region) {
    // Using stripos() since it is much faster then executing preg_match() on every region.
    // Still use preg_match to confirm and count the number of collapsible fieldsets.
    if ( stripos($variables[$region], '<fieldset') !== FALSE && $count = preg_match_all('/<fieldset[^>]+class="[^"]*collapsible/', $variables[$region], $matches)) {
      $number_of_collapsible_fieldsets += $count;
    }
  }

  // Add toggle all. Must execute before $variable['scripts' are regenerated.
  $minimum = variable_get('fieldset_helper_toggle_all_minimum', 2);
  if ($number_of_collapsible_fieldsets >= $minimum) {
    $pages = variable_get('fieldset_helper_toggle_all_pages', 'admin/build/modules
admin/build/modules/list');
    if (!empty($pages) && (trim($pages) == '*' || drupal_match_path($_GET['q'], $pages))) {
      $variables['content'] = fieldset_helper_toggle_all_output() . $variables['content'];
    }
  }

  // Add fieldset_helper js and css.
  if ($number_of_collapsible_fieldsets > 0) {
    // Tao theme theme override theme_fieldset and it uses tao.js to handle collapsible fieldsets.
    if (stripos($variables['scripts'], 'tao.js') === FALSE) {
      drupal_add_js('misc/collapse.js', 'core');
    }
    drupal_add_js( drupal_get_path('module', 'fieldset_helper') .'/fieldset_helper.js');

    // Add ids and cookie duration.
    $settings['fieldset_helper_state_manager'] = array(
      'ids' => fieldset_helper_state_manager_get_lookup_id(),
      'cookie_duration' => variable_get('fieldset_helper_cookie_duration', 0),
    );
    drupal_add_js($settings, 'setting');

    // Reset scripts
    $variables['scripts'] = drupal_get_js();

    // Add and reset css
    drupal_add_css( drupal_get_path('module', 'fieldset_helper') .'/fieldset_helper.css');
    $variables['styles'] = drupal_get_css();
  }
}

// Below preprocess function are only executed if the theme implementation for
// fieldset and fieldgroup are changed from function to a template.
// For example, the tao.theme declares a fieldset.tpl.php.

/**
 * Implementation of hook_preprocess_fieldset().
 */
function fieldset_helper_preprocess_fieldset(&$variables) {
  fieldset_helper_alter_theme_fieldset($variables['element']);
}

/**
 * Implementation of hook_preprocess_fieldgroup().
 */
function fieldset_helper_preprocess_fieldgroup(&$variables) {
  fieldset_helper_alter_theme_fieldset($variables['element']);
}

/**
 * Alters a fieldset element to set default attributes, state, and id.
 *
 * @param &$element
 *   A FAPI fieldset element.
 */
function fieldset_helper_alter_theme_fieldset(&$element) {
  // Make sure each fieldset element is only altered once.
  if ( isset($element['#fieldset_helper_processed']) ) {
    return;
  }

  // Don't set collapsible or collapsed classes for fieldsets that are missing
  // a <legend> (aka #title).
  if (empty($form_element['#title'])) {
    return;
  }

  // Set id for fieldsets without them.
  if ( empty($element['#attributes']['id']) ) {
    if ( isset($element['#attributes']['class']) && preg_match('/group-[-_a-zA-Z]+/', $element['#attributes']['class'], $matches)) {
      $element['#attributes']['id'] = _fieldset_helper_format_id('fieldset-'. $matches[0]);
    }
    else {
      $element['#attributes']['id'] = _fieldset_helper_format_id('fieldset-'. $element['#title']);
    }
  }

  // If fieldset has no classes, add its id as a class.
  // This is extremely useful for styling input format fieldsets.
  if (empty($element['#attributes']['class'])) {
    // Remove form_clean_id incrementing for id.
    $class_name = preg_replace('/-\d+$/', '', $element['#attributes']['id']);
    $element['#attributes']['class'] = $class_name;
  }

  // See if all fieldset are collapsible.
  if ($default_collapsible = fieldset_helper_default_collapsible()) {
    $element['#collapsible'] = $default_collapsible;
  }

  // Set fieldset's default collapsed state
  $element['#collapsed'] = ( isset($element['#collapsed']) ) ? $element['#collapsed'] : FALSE;
  if ($default_collapsed = fieldset_helper_default_collapsed()) {
    $element['#collapsed'] = $default_collapsed;
  }

  // Set fieldset state
  if (user_access('save fieldset state')) {
    $element['#collapsed'] = fieldset_helper_state_manager_get_state($element['#attributes']['id'], $element['#collapsed']);
  }

  // Alter fieldset element.
  drupal_alter('fieldset', $element);

  $element['#fieldset_helper_processed'] = TRUE;
}


////////////////////////////////////////////////////////////////////////////////
// Custom theme preprocess functions
////////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of hook_theme_registry_alter(). Defines fieldset and fieldgroup theme preprocess functions.
 *
 * "Preprocess functions only apply to theming hooks implemented as templates.
 * The main role of the preprocessor is to setup variables to be placed within
 * the template (.tpl.php) files. Plain theme functions do not interact with
 * preprocessors." -- From @link http://drupal.org/node/223430 Setting up variables for use in a template (preprocess functions) @endlink
 *
 * Note: Using *_custom_preprocess instead of *_preprocess because
 * Drupal 7 adds support for theme function preprocessors.
 */
function fieldset_helper_theme_registry_alter(&$theme_registry) {
  $theme_functions = array('fieldset', 'fieldgroup_fieldset');
  foreach ($theme_functions as $theme_function) {
    if (isset($theme_registry[$theme_function]['function'])) {
      $theme_registry[$theme_function]['original_function'] = $theme_registry[$theme_function]['function'];
      $theme_registry[$theme_function]['function'] = 'fieldset_helper_'. $theme_function .'_custom_preprocess';
    }
  }
}

/**
 * Implementation of hook_theme_custom_preprocess().
 */
function fieldset_helper_fieldset_custom_preprocess($element) {
  fieldset_helper_alter_theme_fieldset($element);
  return fieldset_helper_theme_custom('fieldset', $element);
}

/**
 * Implementation of hook_theme_custom_preprocess().
 */
function fieldset_helper_fieldgroup_fieldset_custom_preprocess($element) {
  fieldset_helper_alter_theme_fieldset($element);
  return fieldset_helper_theme_custom('fieldgroup_fieldset', $element);
}

/**
 * Custom handler for theme function that have been hijacked by fieldset_helper_theme_registry_alter().
 */
function fieldset_helper_theme_custom($name, $element = NULL) {
  // Call originally registered theme function.
  $theme_registry = theme_get_registry();
  $theme_function = $theme_registry[$name]['original_function'];

  return call_user_func($theme_function, $element);
}
