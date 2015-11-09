var _crlt_grant_application_review_uids = [];

/**
 * Handles clicks on the assign reviewers checkboxes.
 */
function crlt_grant_application_reviewer_click(input) {
  
  // Grab the new reviewer's user id.
  var uid = jQuery(input).attr('uid');
  if (!uid || uid == '') {
    alert('crlt_grant_application_reviewer_click - Invalid user id!');
    return false;
  }
  
  // Warn user about deleting a reviewer.
  var submit = false;
  if (!$(input).is(':checked')) {
    if ($.inArray(uid, _crlt_grant_application_review_uids) == -1) {
      var msg = 'Are you sure you want to remove this reviewer? ' +
        'This action cannot be undone!';
      if (!confirm(msg)) {
        $(input).attr('checked', true);
        return;
      }
      else { submit = true; }
    }
  }
  else {
    // Set the uid aside as a newly added reviewer, so if its checkbox gets
    // unchecked, we can prevent a deletion attempt.
    _crlt_grant_application_review_uids.push(uid);
  }
  
  // Grab the current user ids.
  var selector = 'form#crlt-grant-application-assign-reviewers-form #edit-field-reviewers';
  var uids = jQuery(selector).val().split(',');
  if (jQuery(selector).val() == '') { uids = []; }
  
  if (jQuery(input).attr('checked')) {
    
    // Make sure they aren't assigning too many reviewers, then add them to the
    // array.
    if (uids.length >= 3) {
      alert('Only 3 reviewers allowed!');
      jQuery(input).attr('checked', false);
      return false;
    }
    uids.push(uid);
    
  }
  else {
    // Remove the uid from the array.
    // @see http://stackoverflow.com/a/3596096/763010
    uids = jQuery.grep(uids, function(value) {
      return value != uid;
    });
  }
  
  if (uids.length == 0) { jQuery(selector).val(''); }
  else {
    //console.log(uids);
    // Place the uids in the text field.
    var value = uids.length == 0 ? '' : uids.join(',');
    jQuery(selector).val(value);
  }
  
  if (submit) {
    $('#crlt-grant-application-assign-reviewers-form').hide('slow');
    $('#crlt-grant-application-assign-reviewers-form input[type="submit"]').click();
  }
  
  //alert();
  
}

