<?php

/**
 * @file
 * Provides a Rules integration for watchdog events.
 */

/**
 * Implementation of hook_rules_event_info().
 *
 * Register rules events used by this module.
 */
function watchdog_rules_rules_event_info() {
  return array(
    'watchdog_rules' => array(
      'label' => t('Watchdog logs an event'),
      'module' => 'Watchdog Rules',
      'arguments' => array(
        'watchdog_user' => array('type' => 'user', 'label' => t('The logged in user when watchdog was called')),
        'watchdog_message' => array('type' => 'watchdog', 'label' => t('The watchdog log message')),
      ),
    ),
  );
}

