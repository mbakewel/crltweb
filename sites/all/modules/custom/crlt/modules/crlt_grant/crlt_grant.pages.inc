<?php

/**
 *
 */
function crlt_grant_report_page($nid) {
  
  $year = isset($_GET['year']) ? $_GET['year'] : date('Y');
  
  $markup = '';
  
  // Load the grant.
  $grant = node_load($nid);
  $markup .= '<h2>' . $grant->title . " - $year</h2><br /><h3>CRLT Staff Reviews</h3>";
  
  // Load the applications for this grant.
  $applications = crlt_grant_applications($nid, $year);
  //drupal_set_message('<pre>' . print_r($applications, true) . '</pre>');
  
  if (empty($applications)) {
    $markup .= '<div class="messages warning">No applications found!</div>';
    return $markup;
  }
  
  // @see admin/content/node-type/grant-application-review/fields/field_grant_application_rating
  $ratings = array(
    1 => 'Low',
    2 => 'Medium',
    3 => 'High'
  );
  
  setlocale(LC_MONETARY, 'en_US');
  
  // Iterate over each application...
  foreach($applications as $application) {
    
    // Pull out some important values.
    $first_name = $application->users_node_data_field_grant_application_users_1_profile_values_profile_first_name_value;
    $last_name = $application->users_node_data_field_grant_application_users_1_profile_values_profile_last_name_value;
    $unit = $application->users_node_data_field_grant_application_users_1_profile_values_unit_value;
    $department = $application->users_node_data_field_grant_application_users_1_profile_values_department_value;
    $overview = $application->node_data_field_grant_app_recommended_fund_field_grant_application_overview_value;
    $budget = str_replace(
      'USD',
      '$',
      money_format('%i', $application->node_node_data_field_grant_node_data_field_grant_type_field_grant_maximum_budget_value)
    );
    
    // Load the application.
    //$application = node_load($app->nid);
    
    // Open the container for this application.
    $markup .= '<div style="margin: 1em 0em 1em 0em;">' .
      "<div><strong>$first_name $last_name</strong> " . str_replace($unit . ' - ', '', $department) . ", $unit</div>" .
      "<div>{$application->node_title}<br />" . strip_tags($overview) . "<br />$budget</div>";
    
    // Load the reviews for this application.
    $reviews = crlt_grant_application_reviews($application->nid);
    //drupal_set_message('<pre>' . print_r($reviews, true) . '</pre>');
    
    // Iterate over each review...
    foreach($reviews as $delta => $review) {
      $reviewer_last_name = $review->profile_values_profile_last_name_value;
      $reviewer_number = $delta + 1;
      $rating = $ratings[$review->node_data_field_grant_application_field_grant_application_rating_value];
      $comment = strip_tags($review->node_revisions_body);
      if (empty($comment)) { $comment = '<em>incomplete</em>'; }
      $markup .= '<div  style="margin: 0.5em 0em 0.5em 0.5em;">' .
        "<div>Reviewer #$reviewer_number: $reviewer_last_name</div>" .
        "<div>Rating: $rating</div>" . 
        "<div>Comment: $comment</div>" .
      '</div>';
    }

    // Close the container.
    $markup .= "</div>";
    
  }
  
  return $markup;

}

