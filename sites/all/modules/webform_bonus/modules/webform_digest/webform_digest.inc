<?php

/**
 * Return periods.
 */
function webform_digest_periods() {
  return array('day' => t('Daily'), 'week' => t('Weekly'), 'month' => t('Monthly'));
}

/**
 * Settings form.
 */
function webform_digest_form(&$form_state, $node) {
  drupal_add_js(drupal_get_path('module', 'webform_digest') . '/webform_digest.js');
  drupal_set_message(t('Cron should be configured to run every hour for correct work of this module.'));
  drupal_set_message(t('Email-to and subject are taken from E-mails configuration tab.'));

  $form = array();

  // Get record from database.
  $digest_conf = db_fetch_array(db_query('SELECT * FROM {webform_digest} WHERE nid=%d', array($node->nid)));
  $digest_conf['digest_settings'] = unserialize($digest_conf['digest_settings']);

  if (empty($digest_conf['sent'])) {
    drupal_set_message(t('Digest have never been sent yet.'));
  } else {
    drupal_set_message(t('Last time digest was sent at !date.', array('!date' => format_date($digest_conf['sent']))));
  }

  // Define form elements.
  $form['enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable digest delivery.'),
    '#default_value' => isset($digest_conf['enabled']) ? $digest_conf['enabled'] : 0,
    '#description' => t('If checkbox is checked, digest will be sent to email addresses specified in the E-mails settings page of Wefborm.'),
  );
  $form['new_data'] = array(
    '#type' => 'checkbox',
    '#title' => t('Send only new data.'),
    '#default_value' => isset($digest_conf['new_data']) ? $digest_conf['new_data'] : 0,
    '#description' => t('If checkbox is checked, digest will include only new data since last email.'),
  );
  $periods = webform_digest_periods();
  $default_period = array_shift(array_keys($periods));
  $form['period'] = array(
    '#type' => 'select',
    '#title' => t('Frequency'),
    '#default_value' => isset($digest_conf['period']) ? $digest_conf['period'] : 0,
    '#options' => $periods,
    '#description' => t('Choose how often digest should be sent.'),
  );
  $form['daily_granularity_month'] = array(
    '#type' => 'select',
    '#title' => t('Day of month'),
    '#default_value' => isset($digest_conf['daily_granularity']) ? $digest_conf['daily_granularity'] : 1,
    '#options' => array_combine(range(1, 31), range(1, 31)),
    '#description' => t('Choose day of month when digest should be sent.'),
  );
  $form['daily_granularity_week'] = array(
    '#type' => 'select',
    '#title' => t('Day of week'),
    '#default_value' => isset($digest_conf['daily_granularity']) ? $digest_conf['daily_granularity'] : 0,
    '#options' => array(
      '0' => t('Sunday'),
      '1' => t('Monday'),
      '2' => t('Tuesday'),
      '3' => t('Wednesday'),
      '4' => t('Thursday'),
      '5' => t('Friday'),
      '6' => t('Saturday'),
    ),
    '#description' => t('Choose day of week when digest should be sent.'),
  );
  $form['hourly_granularity'] = array(
    '#type' => 'select',
    '#title' => t('Hour (24 hour clock)'),
    '#default_value' => isset($digest_conf['hourly_granularity']) ? $digest_conf['hourly_granularity'] : 0,
    '#options' => range(0, 23),
    '#description' => t('Choose time when digest should be sent (24 hour clock).'),
  );
  $form['digest_settings_recieve_digest'] = array(
    '#type' => 'select',
    '#title' => t('E-mails to send digest'),
    '#default_value' => isset($digest_conf['digest_settings']['recieve_digest']) ? $digest_conf['digest_settings']['recieve_digest'] : 0,
    '#options' => _webform_digest_get_email_list($node),
    '#multiple' => true,
    '#description' => t('Select e-mails that should recieve digest regularly.'),
  );
  $form['digest_settings_recieve_submissions'] = array(
    '#type' => 'select',
    '#title' => t('E-mails to send submissions'),
    '#default_value' => isset($digest_conf['digest_settings']['recieve_submissions']) ? $digest_conf['digest_settings']['recieve_submissions'] : 0,
    '#options' => _webform_digest_get_email_list($node),
    '#multiple' => true,
    '#description' => t('Select e-mails that should recieve submissions as they come in.'),
  );
  $form['body'] = array(
    '#type' => 'textarea',
    '#title' => t('E-mail body'),
    '#default_value' => isset($digest_conf['body']) ? $digest_conf['body'] : '',
    '#description' => t('Text that should be sent in e-mail.'),
  );
  $form['sent'] = array(
    '#type' => 'value',
    '#value' => $digest_conf['sent'],
  );


  // Embed download form here and set values for it.
  // Prepare download form state
  $download_form_state = unserialize($digest_conf['settings']);
  // Get download form
  module_load_include('inc', 'webform', 'includes/webform.report');
  $download_form = call_user_func_array('drupal_retrieve_form', array('webform_results_download_form', &$download_form_state, $node));
  // Set default values of fields in download form
  array_walk($download_form, '_webform_digest_set_initial_values', $download_form_state);
  // Add $node to $form_state.
  $download_form_state['values']['node'] = $node;
  // Embed dowload form here.
  $form = array_merge($form, $download_form);

  // Add and update buttons
  $form['submit']['#value'] = t('Save configuration');
  $form['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset'),
  );
  $form['test'] = array(
    '#type' => 'submit',
    '#value' => t('Save and test delivery'),
  );

  return $form;
}

/**
 * Settings form submit.
 */
function webform_digest_form_validate($form, &$form_state) {
  if (empty($form_state['values']['components']['serial'])) {
    form_set_error('serial', t(' Submission Number should be selected in component list.'));
  }
  if (empty($form_state['values']['components']['sid'])) {
    form_set_error('sid', t(' Submission ID should be selected in component list.'));
  }
}

/**
 * Settings form submit.
 */
function webform_digest_form_submit($form, &$form_state) {
  if ($form_state['values']['op'] == $form['submit']['#value']) {
    _webform_digest_form_save($form_state);
  } elseif ($form_state['values']['op'] == $form['reset']['#value']) {
    db_query('DELETE FROM {webform_digest} WHERE nid=%d', array($form_state['values']['node']->nid));
  } elseif ($form_state['values']['op'] == $form['test']['#value']) {
    _webform_digest_form_save($form_state);
    _webform_digest_send_digest($form_state['values']['node']->nid);
  }
}


/**
 * Helper function that saves form state.
 */
function _webform_digest_form_save($form_state) {
  // Create record object to save.
  $digest_conf = new stdClass();

  // Add nid to record.
  $digest_conf->nid = $form_state['values']['node']->nid;

  // Check period and add granularity field to record.
  if (isset($form_state['values']['daily_granularity_' . $form_state['values']['period']])) {
     $digest_conf->daily_granularity = $form_state['values']['daily_granularity_' . $form_state['values']['period']];
  }

  // Add settings field
  $digest_settings['recieve_digest'] = $form_state['values']['digest_settings_recieve_digest'];
  $digest_settings['recieve_submissions'] = $form_state['values']['digest_settings_recieve_submissions'];
  $digest_conf->digest_settings = serialize($digest_settings);

  // Add other fields to record.
  $fields = array('enabled', 'new_data', 'period', 'sent', 'hourly_granularity', 'body');
  foreach ($fields as $field) {
    $digest_conf->$field = $form_state['values'][$field];
  }

  // Unset fields from $form_state. We need to have only download form results here.
  $fields = array('enabled', 'new_data', 'period', 'sent', 'hourly_granularity',
    'daily_granularity_week', 'daily_granularity_month', 'digest_settings_recieve_digest',
    'digest_settings_recieve_submissions', 'node', 'reset', 'test');
  foreach ($fields as $field) {
    unset($form_state['values'][$field]);
    unset($form_state['clicked_button']['#post'][$field]);
  }

  // Unset empty components.
  foreach ($form_state['values']['components'] as $key => $value) {
    if (empty($value)) {
      unset($form_state['values']['components'][$key]);
    }
  }

  // Add download form state
  $digest_conf->settings = serialize($form_state);
    
  // Update record in database.
  db_query('DELETE FROM {webform_digest} WHERE nid=%d', array($digest_conf->nid));
  drupal_write_record('webform_digest', $digest_conf);
}

/**
 * Helper function to set initial values of fields in download form.
 */
function _webform_digest_set_initial_values(&$item, $key, &$form_state) {
  if (is_array($item) && isset($item['#type']) && isset($form_state['values'][$key])) {
    $default_value = $form_state['values'][$key];
    $item['#default_value'] = $default_value;
  } elseif (is_array($item)) {
    array_walk($item, '_webform_digest_set_initial_values', $form_state);
  }
}

/**
 * Send digest.
 */
function _webform_digest_send_digest($nid) {
  module_load_include('inc', 'webform', 'includes/webform.report');

  // Indicates that webform_digest is currently sending emails.
  // Used in webform_digest_nodeapi and in  webform_results_download to override standard behaviour.
  $GLOBALS['webform_digest_send'] = true;

  // Reset node cache, it should contain some overriden information
  $node = node_load($nid, NULL, TRUE);

  // Get record from database.
  $digest_conf = db_fetch_array(db_query('SELECT * FROM {webform_digest} WHERE nid=%d', array($node->nid)));
  $digest_conf['digest_settings'] = unserialize($digest_conf['digest_settings']);
  $download_form_state = unserialize($digest_conf['settings']);

  // Add $node to $form_state.
  $download_form_state['values']['node'] = $node;

  // Replace exporter class. That allows us to filter records in exporter. Class
  // webform_exporter_webform_digest will override current exporter dynamically.
  $exporters = webform_export_fetch_definition();
  $exporter_class_name = $exporters[$download_form_state['values']['format']]['handler'];
  $download_form_state['values']['format'] = 'webform_exporter_webform_digest';

  // Leave only emails that should receive digest
  $node->webform['emails'] = array_intersect_key($node->webform['emails'], $digest_conf['digest_settings']['recieve_digest']);

  // Next lines prepare map for sending email.
  // We need to store information about emails and filters for them.
  $submissions = webform_get_submissions($node->nid);
  foreach ($submissions as $submission) {
    if (!($digest_conf['new_data'] && $submission->submitted < $digest_conf['sent'])) {
      foreach ($node->webform['emails'] as $key => $email_settings) {
        // Process components.
        if (is_numeric($email_settings['email'])) {
          $component = $email_settings['email'];
          if (!empty($submission->data[$component])) {
            foreach ($submission->data[$component]['value'] as $email) {
              if (valid_email_address($email)) {
                $email_map[$key][$email][] = $submission->sid;
              }
            }
          }
        }
        // Process emails
        else if (valid_email_address($email_settings['email']))  {
          $email = $email_settings['email'];
          $email_map[$key][$email][] = $submission->sid;
        }
        // Process multiple emails
        else {
          $emails = explode(',',$email_settings['email']);
          foreach ($emails as $email) {
            if (valid_email_address($email)) {
              $email_map[$key][$email][] = $submission->sid;
            }
          }
        }
      }
    }
  }

  // Prepare exporter configuration array
  $exporter_conf = array();
  $exporter_conf['submissions'] = $submissions;

  // Send digest
  if (!empty($email_map)) {
    foreach ($email_map as $key => $emails) {
      if (empty($emails)) continue;
      foreach ($emails as $email => $sids) {
        $exporter_conf['sids'] = $sids;
        // Set class name for the exporter to override and pass configuration array
        webform_exporter_webform_digest::setClass($exporter_class_name, $exporter_conf);
        _webform_digest_send_digest_email(array($email), $key, $node, $download_form_state, $digest_conf);
      }
    }
  }

  // Unset global variables.
  unset($GLOBALS['webform_digest_send']);

  // Reset node cache which contains some overriden information
  node_load($nid, NULL, TRUE);
}

/**
 * Send digest to specific email. Called by _webform_digest_send_digest().
 */
function _webform_digest_send_digest_email($emails, $key, $node, $download_form_state, $digest_conf) {
  // Intercept download form submission, get file.
  $file_content = '';
  if (ob_start ()) {
    // Submit download form.
    drupal_execute('webform_results_download_form', $download_form_state, $node);
    $file_content = ob_get_contents();
    ob_end_clean();
  }

  // Parse headers.
  $headers = array();
  $headers_arr_str = split("[\n;]", drupal_get_headers());
  foreach ($headers_arr_str as $header_str) {
    $header = split("[=:]", $header_str);
    $headers[trim($header[0])] = trim($header[1]);
  }

  // Prepare email data.
  $email_conf = $node->webform['emails'];
  $sender = array();
  if (!($email_conf[$key]['from_address'] == 'default' || is_numeric($email_conf[$key]['from_address']))) {
    $sender['mail'] = $email_conf[$key]['from_address'];
  }
  if (!($email_conf[$key]['from_name'] == 'default' || is_numeric($email_conf[$key]['from_name']))) {
    $sender['name'] = $email_conf[$key]['from_name'];
  } else {
    $sender['name'] = _webform_filter_values(webform_variable_get('webform_default_from_name'), $node);
  }
  if ($email_conf[$key]['subject'] == 'default' || is_numeric($email_conf[$key]['subject'])) {
    $subject = 'Webform digest: ' . $node->title;
  } else {
    $subject = $email_conf[$key]['subject'];
  }
  $message = $digest_conf['body'];
  $file = array(
    'filename' => $headers['filename'],
    'filemime' => $headers['content-type'],
    'filecontent' => $file_content
  );

  // Send emails
  mimemail($sender, $emails, $subject, $message, true, array(), NULL, array($file), 'webform_digest');
}


/**
 * Get email list.
 */
function _webform_digest_get_email_list($node) {
  $email_list = array();
  $emails = isset($node->webform['emails_all']) ? $node->webform['emails_all'] : $node->webform['emails'];
  foreach ($emails as $eid => $email) {
    $email_addresses = array_filter(explode(',', check_plain($email['email'])));
    foreach ($email_addresses as $key => $email_address) {
      $email_addresses[$key] = webform_format_email_address($email_address, NULL, $node, NULL, FALSE);
    }
    $email_list[$eid] = implode(', ', $email_addresses);
  }
  return $email_list;
}


/*
 * Following class extends any inheritor of webform_exporter.
 * Idea is simmilar to decorator pattern, but decorated object is created in
 * the decorator cunstructor. This class also acts as concrete decorator.
 * Method setClass sets the inheritor class, which will be extended
 * and should be called before decorator object is created.
 */
class webform_exporter_webform_digest extends webform_exporter {
  private static $className;
  private static $conf;
  private $object;

  // Saves class name of the object to be extended and conf array to static variables.
  static public function setClass($className, $conf = null) {
    self::$className = $className;
    self::$conf = $conf;
  }

  function __construct($options) {
    // Create object, which is extended
    $this->object = new self::$className($options);
    // Initialize variables of concrete decorator
    if (isset(self::$conf['sids'])) {
      $this->sids = self::$conf['sids'];
    }
    if (isset(self::$conf['submissions'])) {
      $this->submissions = self::$conf['submissions'];
    }
    // Save index of sid in components array
    if ($sid = array_search('sid', $options['components'])) {
      $this->sid_index = $sid;
    }
    if (array_search('info', $options['components'])) {
      $this->sid_index--;
    }
  }

  function add_row(&$file_handle, $data) {
    // Check if sid is assigned.
    if (isset($this->sid_index)) {
      // Filter records
      $sid = $data[$this->sid_index];
      if ((!isset($this->submissions[$sid]) || in_array($sid, $this->sids))) {
        return $this->object->add_row($file_handle, $data);
      }
    }
  }

  function set_headers($filename) {
    return $this->object->set_headers($filename);
  }

  function bof(&$file_handle) {
    return $this->object->bof($file_handle);
  }

  function eof(&$file_handle) {
    return $this->object->eof($file_handle);
  }
}
