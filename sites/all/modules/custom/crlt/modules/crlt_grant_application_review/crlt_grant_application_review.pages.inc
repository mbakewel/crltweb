<?php

/**
 *
 */
function crlt_grant_application_reviews_user_page($uid) {
  
  return 'DEPRECATED!';
  
  $markup = '';
  
  // Helpful links.
  $markup .= theme('item_list', array(
    l('Back to all Grant Applications', 'admin/crlt/grant-applications')
  ));
  
  // Load the user's account and their reviews.
  $account = user_load($uid);
  $reviews = crlt_grant_application_reviews_user($uid);
  //drupal_set_message('<pre>' . print_r($reviews, true) . '</pre>');
  
  if (!empty($reviews)) {
    $header = array('Application', 'Actions');
    $rows = array();
    foreach($reviews as $row) {
      $rows[] = array(
        $row->node_title,
        l('view', "node/{$row->nid}") . ' - ' .
        l('edit', "node/{$row->nid}/edit") . ' - ' .
        l('reviewers', "admin/crlt/grant-applications/assign-reviewers/{$row->node_node_data_field_grant_application_nid}")
      );
    }
    $markup .= theme('table', $header, $rows);
  }
  else {
    $markup .= '<div class="messages warning">No applications assigned for review!</div>';
  }
  
  return $markup;
}

