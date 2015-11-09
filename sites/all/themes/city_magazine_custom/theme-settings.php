<?php  
// $Id$
/**
 * @file
 * theme-settings.php
 */
// Include the definition of city_magazine_settings() and fusion_theme_get_default_settings().
include_once './'. drupal_get_path('theme', 'city_magazine') .'/theme-settings.php';

 
/**
 * Implementation of THEMEHOOK_settings() function.
 *
 * @param $saved_settings
 *   An array of saved settings for this theme.
 * @return
 *   A form array. 
 */
function city_magazine_custom_settings($saved_settings) {
  $form = array();
  // Get the default values from the .info file.
  $defaults = fusion_core_default_theme_settings('city_magazine_custom');
  $settings = array_merge($defaults, $saved_settings);

  // Add the base theme's settings.
  $form += city_magazine_settings($saved_settings, $defaults);

  // Return the form 
  return $form;
}