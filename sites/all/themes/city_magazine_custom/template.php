<?php  
// $Id$

/**		
 * @file
 * Contains theme override functions and preprocess functions for the theme.
 *
 * ABOUT THE TEMPLATE.PHP FILE
 *
 *   The template.php file is one of the most useful files when creating or
 *   modifying Drupal themes. You can add new regions for block content, modify
 *   or override Drupal's theme functions, intercept or make additional
 *   variables available to your theme, and create custom PHP logic. For more
 *   information, please visit the Theme Developer's Guide on Drupal.org:
 *   http://drupal.org/theme-guide
 *
 * OVERRIDING THEME FUNCTIONS
 *
 *   The Drupal theme system uses special theme functions to generate HTML
 *   output automatically. Often we wish to customize this HTML output. To do
 *   this, we have to override the theme function. You have to first find the
 *   theme function that generates the output, and then "catch" it and modify it
 *   here. The easiest way to do it is to copy the original function in its
 *   entirety and paste it here, changing the prefix from theme_ to city_magazine_custom_.
 *   For example:
 *
 *     original: theme_breadcrumb()
 *     theme override: city_magazine_custom_breadcrumb()
 *
 *   where city_magazine_custom is the name of your sub-theme.
 *
 *   If you would like to override any of the theme functions used in city_magazine core,
 *   you should first look at how Comaco core implements those functions:
 *     theme_breadcrumbs()      in city_magazine/template.php
 *     theme_menu_item_link()   in city_magazine/template.php
 *     theme_menu_local_tasks() in city_magazine/template.php
 *
 *   For more information, please visit the Theme Developer's Guide on
 *   Drupal.org: http://drupal.org/node/173880
 *
 * CREATE OR MODIFY VARIABLES FOR YOUR THEME
 *
 *   Each tpl.php template file has several variables which hold various pieces
 *   of content. You can modify those variables (or add new ones) before they
 *   are used in the template files by using preprocess functions.
 *
 *   This makes THEME_preprocess_HOOK() functions the most powerful functions
 *   available to themers.
 *
 *   It works by having one preprocess function for each template file or its
 *   derivatives (called template suggestions). For example:
 *     THEME_preprocess_page    alters the variables for page.tpl.php
 *     THEME_preprocess_node    alters the variables for node.tpl.php or
 *                              for node-forum.tpl.php
 *     THEME_preprocess_comment alters the variables for comment.tpl.php
 *     THEME_preprocess_block   alters the variables for block.tpl.php
 *
 *   For more information on preprocess functions and template suggestions,
 *   please visit the Theme Developer's Guide on Drupal.org:
 *   http://drupal.org/node/223440
 *   and http://drupal.org/node/190815#template-suggestions
 */


/*
 * Add any conditional stylesheets you will need for this sub-theme.
 *
 * To add stylesheets that ALWAYS need to be included, you should add them to
 * your .info file instead. Only use this section if you are including
 * stylesheets based on certain conditions.
 */


/**
 * Implementation of HOOK_theme().
 */
function city_magazine_custom(&$existing, $type, $theme, $path) {
  $hooks = city_magazine($existing, $type, $theme, $path);
  // Add your theme hooks like this:
  /*
  $hooks['hook_name_here'] = array( // Details go here );
  */
  // @TODO: Needs detailed comments. Patches welcome!
  return $hooks;
}

/**
 * Override or insert variables into all templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered (name of the .tpl.php file.)
 */
/* -- Delete this line if you want to use this function
function city_magazine_custom_preprocess(&$vars, $hook) {
  $vars['sample_variable'] = t('Lorem ipsum.');
}
// */

/**
 * Override or insert variables into the page templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("page" in this case.)
 */
/* -- Delete this line if you want to use this function
function city_magazine_custom_preprocess_page(&$vars, $hook) {
  $vars['sample_variable'] = t('Lorem ipsum.');
}
// */

/**
 * Override or insert variables into the node templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("node" in this case.)
 */
/* -- Delete this line if you want to use this function
function city_magazine_custom_preprocess_node(&$vars, $hook) {
  $vars['sample_variable'] = t('Lorem ipsum.');
}
// */

/**
 * Override or insert variables into the comment templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("comment" in this case.)
 */
/* -- Delete this line if you want to use this function
function city_magazine_custom_preprocess_comment(&$vars, $hook) {
  $vars['sample_variable'] = t('Lorem ipsum.');
}
// */

/** 
 * Override or insert variables into the block templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("block" in this case.)
 */
 /* -- Delete this line if you want to use this function
function city_magazine_custom_preprocess_block(&$vars, $hook) {
  $vars['sample_variable'] = t('Lorem ipsum.');
}
// */

function phptemplate_signup_user_form($node) {
  global $user;
  $form = array();
  if (variable_get('signup_ignore_default_fields', 0)) {
    return $form;
  }
  // If this function is providing any extra fields at all, the following
  // line is required for form form to work -- DO NOT EDIT OR REMOVE.
  $form['signup_form_data']['#tree'] = TRUE;

  /*$admin = false;
  if ($user->uid != 0 && in_array('admin', $user->roles) || in_array('CRLT Staff', $user->roles)) {
    //drupal_set_message('<pre>' . print_r($node->webform, true) . '</pre>');
    //drupal_set_message('<pre>' . print_r($node, true) . '</pre>');
    $admin = true;
  }*/

  // Place user profile fields on the form. If we're
  // editing a signup, use the account of the user on the signup. If we're on the
  // admin signup form, set the account to false so the profile fields won't be
  // populated with the admin's data.
  $account = null;
  if (arg(0) == 'signup' && arg(1) == 'edit' && is_numeric(arg(2))) {
    $sql = "SELECT uid FROM signup_log WHERE sid = %d";
    $uid = db_result(db_query($sql, arg(2)));
    if ($uid) {
      $account = user_load($uid);
    }
  } 
  else if (arg(0) == 'node' && arg(2) == 'signups' && arg(3) == 'add') {
    $account = false;
  }
  // admin/user/profile
  $sql = "SELECT * FROM {profile_fields} WHERE fid IN (2,3,4,5,6) ORDER BY weight ASC ";
  $elements = crlt_generate_user_profile_field_form_elements($sql, $account);
  foreach($elements as $name => $element) {
    $form['signup_form_data'][$name] = $element; 
  }

  // If there are any webform components attached to this node, place the
  // components on the signup form. 
  if (isset($node->webform) && !empty($node->webform['components'])) {
    foreach($node->webform['components'] as $cid => $component) {
      $element = null;
      if ($component['type'] == 'markup') {
        $title = $component['name'] ? '<h4>' . $component['name']  . '</h4>' : '';
        $element = array(
          '#value' => $title . $component['value']
        );
      }
      else {
        $type = $component['type'];
        if ($type == 'select') {
          $type = 'checkboxes';
          if (isset($component['extra']['aslist']) && $component['extra']['aslist'] == 1) {
            $type = 'select';
          }
          else if (!$component['extra']['multiple']) {
            $type = 'radios'; 
          }
        }
        $element = array(
          '#title' => $component['name'],
          '#type' => $type,
          '#required' => $component['mandatory'] ? TRUE: FALSE,
          '#default_value' => $component['value']
        );
      }
      // Add options to the element if necessary. The options are stored
      // as items separated by key|label and by new lines.
      if (isset($component['extra']['items'])) {
        $element['#options'] = array();
        foreach(preg_split("/((\r?\n)|(\r\n?))/", $component['extra']['items']) as $item){
          $parts = explode('|', $item);
          if (sizeof($parts) != 2) { continue; }
          $key = $parts[0];
          $value = $parts[1];
          $element['#options'][$key] = $value;
        } 
      }
      // File elements cannot be required.
      if ($component['type'] == 'file') {
        $element['#required'] = FALSE;
      }
      // Attach the component to the form as an element.
      $form['signup_form_data'][$component['form_key']] = $element;
    }
  }

  // Comments.
  $form['signup_form_data']['comments'] = array(
    '#type' => 'textarea',
    '#title' => t('Comments'),
    '#wysiwyg' => false,
  );

  return $form;

}

function city_magazine_custom_preprocess_page(&$variables) {
  if($variables['node']->type != "") {
    $variables['template_files'][] = 'page-node-' . $variables['node']->type;
  }


}

function city_magazine_custom_date_all_day_label() {
	return '(no time specified)';
}

function city_magazine_custom_form_element($element, $value) {
  $output  = '<div class="form-item"';
  if (!empty($element['#id'])) {
    $output .= ' id="'. $element['#id'] .'-wrapper"';
  }
  $output .= ">\n";
  $required = !empty($element['#required']) ? '<span class="form-required" title="'. t('This field is required.') .'">*</span>' : '';
  if (!empty(

$element['#title'])) {
    $title = $element['#title'];
    if (!empty($element['#id'])) {
      $output .= ' <label for="'. $element['#id'] .'">'. t('!title: !required', array('!title' => filter_xss_admin($title), '!required' => $required)) ."</label>\n";
    }
    else {
      $output .= ' <label>'. t('!title: !required', array('!title' => filter_xss_admin($title), '!required' => $required)) ."</label>\n";
    }
  }
  if (!empty(

$element['#description'])) {
    $output .= ' <div class="description">'. $element['#description'] ."</div>\n";
  }
  

$output .= " $value\n";
  

$output .= "</div>";
  return 

$output;
}

function city_magazine_custom_content_multiple_values($element) {
  $field_name = $element['#field_name'];
  $field = content_fields($field_name);
  $output = '';
  if (

$field['multiple'] >= 1) {
    $table_id = $element['#field_name'] .'_values';
    $order_class = $element['#field_name'] .'-delta-order';
    $required = !empty($element['#required']) ? '<span class="form-required" title="'. t('This field is required.') .'">*</span>' : '';
    

$header = array(
      array(
        'data' => t('!title: !required', array('!title' => $element['#title'], '!required' => $required)).'<br /><div class="description">'. $element['#description'] .'</div>',
        'colspan' => 2
      ),
      t('Order'),
    );
    $rows = array();
    

// Sort items according to '_weight' (needed when the form comes back after
    // preview or failed validation)
    $items = array();
    foreach (element_children($element) as $key) {
      if ($key !== $element['#field_name'] .'_add_more') {
        $items[] = &$element[$key];
      }
    }
    usort($items, '_content_sort_items_value_helper');
    

// Add the items as table rows.
    foreach ($items as $key => $item) {
      $item['_weight']['#attributes']['class'] = $order_class;
      $delta_element = drupal_render($item['_weight']);
      $cells = array(
        array('data' => '', 'class' => 'content-multiple-drag'),
        drupal_render($item),
        array('data' => $delta_element, 'class' => 'delta-order'),
      );
      $rows[] = array(
        'data' => $cells,
        'class' => 'draggable',
      );
    }
    $output .= theme('table', $header, $rows, array('id' => $table_id, 'class' => 'content-multiple-table'));
    $output .= drupal_render($element[$element['#field_name'] .'_add_more']);
    

drupal_add_tabledrag($table_id, 'order', 'sibling', $order_class);
  }
  else {
    foreach (element_children($element) as $key) {
      $output .= drupal_render($element[$key]);
    }
  }
  return $output;
}

// remove the revision information field from displaying for non moderator roles
function city_magazine_custom_node_form($form) {
  // Remove 'Log message' text area
$form['revision_information']['#access'] = user_access('view revisions');
   return theme_node_form($form);
}
