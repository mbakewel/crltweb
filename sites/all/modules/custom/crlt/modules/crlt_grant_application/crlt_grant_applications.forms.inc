<?php

/**
 *
 */
function crlt_grant_application_assign_reviewers_form( &$form_state, $nid ) {

  global $user;

  // @TODO - this needs to be across all the grant applications, not just for a
  // signle grant applications.

  $form = array();

  // Load the grant application node and place its node id in the form as a
  // hidden element.
  $node = node_load( $nid );
  if ( !$node || $node->type != 'grant_application' ) {
    drupal_set_message( 'Invalid grant application node: ' . $nid, 'error' );
    return $form;
  }
  $form['nid'] = array(
    '#type' => 'hidden',
    '#value' => $nid
  );

  // Load the grant node.
  $grant = node_load( $node->field_grant[0]['nid'] );

  // Load the primary applicant.
  $account = user_load( $node->field_grant_application_users[0]['uid'] );

  // Add some useful links.
  $form['#prefix'] = theme( 'item_list', array(
      l( 'Back to all Grants', 'admin/crlt/grants' ),
      l( 'Back to all Grant Applications', 'admin/crlt/grant-applications' ),
      l( 'View My Grant Application Reviews', "user/{$user->uid}/grant-application-reviews" ),
      l( 'View Grant', "node/$nid" ) . " - ({$grant->title})",
      l( 'View Grant Application', "node/$nid" ) .
      " - (by: <strong>{$account->profile_last_name} | {$node->field_grant_application_year[0]['value']})</strong>"
    ) );


  // Load the reviews (if any) for this application. We determine the status of
  // the review by looking at the review node's body. If it is not empty, then
  // we assume the review is complete.
  $uids = array();
  $reviews = crlt_grant_application_reviews( $nid );
  //drupal_set_message('<pre>' . print_r($reviews, true) . '</pre>');
  if ( !empty( $reviews ) ) {
    $headers = array(
      '&nbsp;',
      'Assigned reviewer',
      'Application review status',
      'Application review actions'
    );
    $rows = array();
    $destination = drupal_get_destination();
    foreach ( $reviews as $delta => $row ) {
      $status = !empty( $row->node_revisions_body ) ? 'complete' : '<em>incomplete</em>';
      $uid = $row->users_uid;
      $first_name = $row->profile_values_profile_first_name_value;
      $last_name = $row->profile_values_profile_last_name_value;
      $uids[] = $uid;
      $rows[] = array(
        "<em>[" . l( 'x', "node/{$row->nid}/delete", array(
            'query' => $destination,
            'attributes' => array( 'title' => "Remove $first_name from this application" )
          ) ) . "]</em>",
        l(
          $last_name . ', ' . $first_name,
          "user/{$uid}/grant-application-reviews",
          array( 'attributes' => array( 'title' => "View $first_name's Application Reviews" ) )
        ) .
        " ({$row->users_name})",

        $status,

        l( 'view', "node/{$row->nid}" ) . ' - ' .
        l( 'edit', "node/{$row->nid}/edit", array( 'query' => $destination ) )

      );
    }
    $markup = theme( 'table', $headers, $rows );
  }
  else {
    $markup = '<div class="messages warning">No reviewers assigned yet!</div>';
  }

  // Show available reviewers table widget.
  $form['reviews'] = array(
    '#value' => $markup . crlt_grant_application_review_assignment_widget(
      $nid,
      $uids,
      $node->field_grant_application_year[0]['value'],
      $grant->field_grant_type[0]['value']
    )
  );

  // Submit button.
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t( 'Update reviewers' )
  );

  // Make a text field (only visible to super admins) that will house a comma
  // separated list of user ids that will review the current application. The
  // widget uses JS to auto populate this field for us, then we just handle it
  // in this form's submission handler.
  $form['field_reviewers'] = array(
    '#type' => 'textfield',
    '#title' => 'Grant Application Reviewer User IDs',
    '#default_value' => implode( ',', $uids ),
    '#description' => 'This field will be auto populated by the widget. Separate multiple user ids by comma.'
  );
  if ( $user->uid == 1 || $user->name == 'tjfrank' ) {
    $form['field_reviewers']['#prefix'] = '<div class="messages">This is only visible to super admins.</div>';
  }
  else {
    $form['field_reviewers']['#prefix'] = '<div style="display: none;">';
    $form['field_reviewers']['#suffix'] = '</div>';
  }

  return $form;
}

/**
 * The CRLT Event Settings Form's Submit Handler.
 */
function crlt_grant_application_assign_reviewers_form_submit( $form, &$form_state ) {
  //drupal_set_message('<pre>' . print_r($form_state['values'], true) . '</pre>');

  $nid = $form_state['values']['nid'];

  // Load the application.
  $node = node_load( $nid );
  //drupal_set_message("Loaded {$node->title}");

  // Extract the reviewer user ids.
  $reviewers = explode( ',', $form_state['values']['field_reviewers'] );
  $reviewers_count = sizeof( $reviewers );
  //drupal_set_message('<pre>' . print_r($reviewers, true) . '</pre>');

  // Load the reviews.
  $reviews = crlt_grant_application_reviews( $nid );
  $review_count = sizeof( $reviews );
  //drupal_set_message('<pre>' . print_r($reviews, true) . '</pre>');

  // See if any reviews have been removed?
  foreach ( $reviews as $review ) {
    if ( !in_array( $review->users_uid, $reviewers ) ) {
      node_delete( $review->nid );
      drupal_set_message( "Removed {$review->profile_values_profile_first_name_value} from {$node->title}." );
    }
  }

  // Process each reviewer...
  foreach ( $reviewers as $uid ) {

    // Load their user account.
    $account = user_load( $uid );

    // Locate their review, or create a new one if it doesn't exist.
    $found = FALSE;
    foreach ( $reviews as $delta => $row ) {
      if ( $row->users_uid == $account->uid ) {
        //drupal_set_message("Found {$account->name}'s review");
        $found = TRUE;
        break;
      }
    }
    if ( !$found ) {
      $review = new stdClass();
      $review->type = 'grant_application_review';
      $review->uid = $uid;
      $review->title = $account->profile_first_name . ' ' . $account->profile_last_name .
        "'s review of " . $node->title;
      $review->field_grant_application = array( array( 'nid' => $nid ) );
      //drupal_set_message('<pre>' . print_r($review, true) . '</pre>');
      node_save( $review );
      if ( $review->nid ) {
        drupal_set_message( "Assigned application review to {$account->profile_first_name}." );
      }
    }

  }

}
