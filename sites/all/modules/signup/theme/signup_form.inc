<?php
// $Id: signup_form.inc,v 1.3.2.3 2010/12/28 17:28:47 ezrag Exp $


/**
 * @file
 * Site-specific code related to the form when users signup for a node.
 */

/**
 * Return the site-specific custom fields for the signup user form.
 *
 * To customize this for your site, copy this entire function into
 * your theme's template.php file, rename the function to
 * phptemplate_signup_user_form(), and modify to taste.  Feel free to
 * alter any elements in this section, remove them, or add any others.
 *
 * WARNING: If your site allows anonymous signups and you alter the
 * 'Name' field in this function, you will probably have to implement a
 * version of theme_signup_anonymous_username() for your site.
 *
 * In order for the form to be rendered properly and for the custom
 * fields to be fully translatable when printed in other parts of the
 * Signup module (displayed in signup lists, emails, etc), the name of
 * the form element must be $form['signup_form_data']['NameOfDataField'],
 * where NameOfDataField is replaced with the actual name of the data
 * field.  For translation to work, the displayed name of the field
 * (the '#title' property) be the same as the name of the data field,
 * but wrapped in t().  See below for examples.
 *
 * Fieldsets are not currently supported in this form.  Any
 * '#default_value' will be filled in by default when the form is
 * presented to the user.  Any field marked '#required' must be filled
 * in before the user can sign up.
 *
 * If you do not want any additional fields, the function can simply
 * return an empty array: "return array();"
 *
 * @param $node
 *   The fully loaded node object where this signup form is appearing.
 *
 * @return
 *   Array defining the form to present to the user to signup for a node.
 *
 * @see theme_signup_anonymous_username()
 */
function theme_signup_user_form($node) {
  // Overridden in ./sites/all/themes/city_magazine_custom/template.php
  global $user;
  $form = array();
  if (variable_get('signup_ignore_default_fields', 0)) {
    return $form;
  }
  // If this function is providing any extra fields at all, the following
  // line is required for form form to work -- DO NOT EDIT OR REMOVE.
  $form['signup_form_data']['#tree'] = TRUE;

  //// Begin custom fields

  // Get uniqname
  $form['signup_form_data']['Name'] = array(
    '#type' => 'hidden',
    '#title' => t('Uniqname'),
    '#size' => 10, '#maxlength' => 20,
  );

  module_load_include('inc', 'hierarchical_select', 'includes/common');

  $vocabulary = taxonomy_vocabulary_load(5);


  $options = array();
  $options['no choice'] = '- Please choose -';
  $form['signup_form_data']['status'] = array(
    '#type' => 'select',
    '#title' => t('Primary Appointment2'),
    '#options' => $options,
  );

  $options = array();
  $options['no choice'] = '- Please choose -';
  $form['signup_form_data']['status'] = array(
    '#type' => 'select',
    '#title' => t('Primary Appointment'),
    '#options' => $options,
  );

  $options = array();
  $options['no choice'] = '- Please choose -';
  $form['signup_form_data']['unit'] = array(
    '#type' => 'select',
    '#title' => t('Unit'),
    '#options' => $options,
  );

  $options = array();
  $options['no choice'] = '- Choose one, if in Administration, LS&A or Engineering -';
  $form['signup_form_data']['dept'] = array(
    '#type' => 'select',
    '#title' => t('Department'),
    '#options' => $options,
  );

  $form['signup_form_data']['regfirstname'] = array(
    '#type' => 'textfield',
    '#title' => t('First Name'),
    '#size' => 40, '#maxlength' => 64,
    '#required' => TRUE,
  );

  $form['signup_form_data']['reglastname'] = array(
    '#type' => 'textfield',
    '#title' => t('Last Name'),
    '#size' => 40, '#maxlength' => 64,
    '#required' => TRUE,
  );

  $form['signup_form_data']['useplan'] = array(
    '#type' => 'textarea',
    '#title' => t('How do you plan to use this technology in your teaching?'),
    '#wysiwyg' => false,
  );

  $form['signup_form_data']['learningneed'] = array(
    '#type' => 'textarea',
    '#title' => t('What do you need to learn to take the next step?'),
    '#wysiwyg' => false,
  );
  
  $form['signup_form_data']['comments'] = array(
    '#type' => 'textarea',
    '#title' => t('Comments'),
    '#wysiwyg' => false,
  );

  // End custom fields

  // If the user is logged in, fill in their name by default.
  if ($user->uid) {
    $form['signup_form_data']['Name']['#default_value'] = $user->name;
  }

  return $form;
}

/**
 * Returns the value to use for the user name for anonymous signups.
 *
 * WARNING: If you implemented your own version of theme_signup_form_data()
 * that changed or removed the custom 'Name' field and your site
 * allows anonymous signups, you will need to modify this, too.
 *
 * This value is used for the %user_name email token for anonymous users, and
 * also to identify a particular anonymous signup in various places in the UI.
 *
 * @param $form_data
 *   Array of custom signup form values for the current signup.
 * @param $email
 *   E-mail address of the anonymous user who signed up.
 * @return
 *   A string with the proper value for the %user_name email token.
 *
 * @see theme_signup_user_form()
 */
function theme_signup_anonymous_username($form_data, $email) {
  // In some cases, the best you can do is to use the anonymous user's
  // supplied email address, in which case, you should uncomment this:
  //return $email;

  // WARNING: This line is only valid if you left the 'Name' field in
  // your site's version of theme_signup_user_form().
  return $form_data['Name'];
}

