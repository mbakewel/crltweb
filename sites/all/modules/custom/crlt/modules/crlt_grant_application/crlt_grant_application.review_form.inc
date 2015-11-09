<?php
/**
 * drupal_get_form form generation hook
 *
 * Form used for visibility and editing of grant application reporting fields
 */
function crlt_grant_application_report_form( &$form_state, $nid ) {
  $form = array();
  // Load the grant application node
  $node = node_load( $nid, NULL, TRUE );
  // Get submitter
  $submitter = user_load( array( 'uid'=>$node->uid ) );
  // Try to get first applicant
  if ( isset( $node->field_grant_application_users[0]['uid'] ) ) {
    $applicant = user_load( array( 'uid'=>$node->field_grant_application_users[0]['uid'] ) );
  } else {
    // Default to node submitter as applicant, if needed
    $applicant = $submitter;
  }
  // Try to get grant
  if ( isset( $node->field_grant[0]['nid'] ) ) {
    $grant = node_load( array( 'nid'=>$node->field_grant[0]['nid'] ) );
    $grant_title = $grant->title;
  } else {
    $grant_title = 'grant';
  }
  // Try to get grant year
  if ( isset( $node->field_grant_application_year[0]['value'] ) ) {
    $grant_year = $node->field_grant_application_year[0]['value'];
  } else {
    $grant_year = '';
  }
  // Only support Grant Application node types
  if ( $node || 'grant_application' == $node->type ) {
    // Pass the nid as a value
    $form['nid'] = array(
      '#type' => 'value',
      '#default_value' => $nid
    );
    $form['intro'] = array(
      '#value' => sprintf(
        '<h2 class="crlt-grant crlt-grant-title">%s</h2><p class="crlt-grant crlt-grant-name"><strong>Grant:</strong> %s %s</p><p class="crlt-grant crlt-grant-name"><strong>Applicant:</strong> %s %s (%s)</p><p class="crlt-grant crlt-grant-submitter"><strong>Submitter:</strong> %s %s (%s)</p><p class="crlt-grant-guide">%s</p>',
        $node->title,
        $grant_year,
        $grant_title,
        $applicant->profile_first_name,
        $applicant->profile_last_name,
        $applicant->name,
        $submitter->profile_first_name,
        $submitter->profile_last_name,
        $submitter->name,
        t( 'Please complete the following CRLT Grant reporting form.' )

      ),
    );
    // Grant Report fields
    $form['field_grant_app_proj_objectives'] = array(
      '#type' => 'text_textarea',
      '#title' => t( 'Project Objectives' ),
      '#default_value' => $node->field_grant_app_proj_objectives[0],
      '#tree' => true,
    );
    $form['field_grant_app_proj_achievement'] = array(
      '#type' => 'text_textarea',
      '#title' => t( 'Project Achievements' ),
      '#default_value' => $node->field_grant_app_proj_achievement[0],
      '#tree' => true,
    );
    $form['field_grant_app_continuation'] = array(
      '#type' => 'text_textarea',
      '#title' => t( 'Continuation' ),
      '#default_value' => $node->field_grant_app_continuation[0],
      '#tree' => true,
    );
    $form['field_grant_app_dissemination'] = array(
      '#type' => 'text_textarea',
      '#title' => t( 'Dissemination' ),
      '#default_value' => $node->field_grant_app_dissemination[0],
      '#tree' => true,
    );
    $form['field_grant_app_colleague_advice'] = array(
      '#type' => 'text_textarea',
      '#title' => t( 'Advice to your Colleagues' ),
      '#default_value' => $node->field_grant_app_colleague_advice[0],
      '#tree' => true,
    );
    $form['field_grant_app_crlt_advice'] = array(
      '#type' => 'text_textarea',
      '#title' => t( 'Advice to CRLT' ),
      '#default_value' => $node->field_grant_app_crlt_advice[0],
      '#tree' => true,
    );
    // Submit button.
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t( 'Save report' )
    );
  } else {
    drupal_set_message( 'Invalid grant application node: ' . $nid, 'error' );
  }

  return $form;
}
/**
 * drupal_get_form form submission hook
 *
 * Form used for visibility and editing of grant application reporting fields
 */
function crlt_grant_application_report_form_submit( $form, &$form_state ) {
  // drupal_set_message( sprintf( '<pre>%s</pre>', htmlspecialchars( print_r( $form_state, TRUE ) ) ) );
  $nid = $form_state['values']['nid'];

  // Load the grant application
  $node = node_load( $nid );
  // Set grant application fields from form
  $node->field_grant_app_proj_objectives[0]  = $form_state['values']['field_grant_app_proj_objectives'];
  $node->field_grant_app_proj_achievement[0] = $form_state['values']['field_grant_app_proj_achievement'];
  $node->field_grant_app_continuation[0]     = $form_state['values']['field_grant_app_continuation'];
  $node->field_grant_app_dissemination[0]    = $form_state['values']['field_grant_app_dissemination'];
  $node->field_grant_app_colleague_advice[0] = $form_state['values']['field_grant_app_colleague_advice'];
  // drupal_set_message(sprintf('<h1>$node->field_grant_app_crlt_advice[0]</h1><pre>%s</pre>',htmlspecialchars(print_r($node->field_grant_app_crlt_advice[0],TRUE))));
  // drupal_set_message(sprintf('<h1>$node-pre</h1><pre>%s</pre>',htmlspecialchars(print_r($form_state['values']['field_grant_app_crlt_advice'],TRUE))));
  $node->field_grant_app_crlt_advice[0]      = $form_state['values']['field_grant_app_crlt_advice'];
  // drupal_set_message(sprintf('<h1>$form_state[\'values\'][\'field_grant_app_crlt_advice\']</h1><pre>%s</pre>',htmlspecialchars(print_r($node,TRUE))));
  // Save the grant application
  node_save( $node );
  // drupal_set_message(sprintf('<h1>$node-post</h1><pre>%s</pre>',htmlspecialchars(print_r($node,TRUE))));

}
