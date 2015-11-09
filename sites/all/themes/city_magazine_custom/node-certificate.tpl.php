<?php
// $Id: node.tpl.php
?>
<?php
  // Print out the node's workflow history for admins and the certificate
  // author, except on a node teaser.
  // This is pretty much a verbatim copy of workflow_tab_page() in workflow.pages.inc,
  // except we don't use the majority of its theme() calls since it is causing
  // problems, instead we replaced those lines with manual calls.
  global $user;
  if (!$teaser &&
    $user->uid != 0 && (
      $user->uid == $node->uid || (
        in_array('admin', $user->roles) ||
        in_array('CRLT Staff', $user->roles)
      )
    )
  ) {
    // Print a cancellation link, if it isn't already cancelled.
    if ($node->_workflow != 12) {
      print l('Cancel Certificate', 'crlt/certificate/cancel/' . $node->nid, array('query' => array('destination' => implode('/', arg())))) . '<br />';
    }
    $wid = workflow_get_workflow_for_type($node->type);
    $states_per_page = 250;
    $states = array();
    $result = db_query("SELECT sid, state FROM {workflow_states} WHERE status = 1 ORDER BY sid");
    while ($data = db_fetch_object($result)) {
      $states[$data->sid] = check_plain(t($data->state));
    }
    $deleted_states = array();
    $result = db_query("SELECT sid, state FROM {workflow_states} WHERE status = 0 ORDER BY sid");
    while ($data = db_fetch_object($result)) {
      $deleted_states[$data->sid] = check_plain(t($data->state));
    }
    $current = workflow_node_current_state($node);
  
    // theme_workflow_current_state() must run state through check_plain().
    $output = '<p>'. t('Current state: <strong>!state</strong>', array('!state' => $states[$current])) . "</p>\n";
  
    $result = pager_query("SELECT h.*, u.name FROM {workflow_node_history} h LEFT JOIN {users} u ON h.uid = u.uid WHERE nid = %d ORDER BY hid DESC", $states_per_page, 0, NULL, $node->nid);
    $rows = array();
    while ($history = db_fetch_object($result)) {
      if ($history->sid == $current && !isset($deleted_states[$history->sid]) && !isset($current_themed)) {
        // Theme the current state differently so it stands out.
        $state_name = $states[$history->sid];
        // Make a note that we have themed the current state; other times in the history
        // of this node where the node was in this state do not need to be specially themed.
        $current_themed = TRUE;
      }
      elseif (isset($deleted_states[$history->sid])) {
        // The state has been deleted, but we include it in the history.
        $state_name = $deleted_states[$history->sid];
        $footer_needed = TRUE;
      }
      else {
        // Regular state.
        $state_name = check_plain(t($states[$history->sid]));
      }
  
      if (isset($deleted_states[$history->old_sid])) {
        $old_state_name = $deleted_states[$history->old_sid];
        $footer_needed = TRUE;
      }
      else {
        $old_state_name = check_plain(t($states[$history->old_sid]));
      }
      $row = array(
        format_date($history->stamp),
        $old_state_name,
        $state_name,
        $history->name,
        filter_xss($history->comment, array('a', 'em', 'strong')),
      );
      $rows[] = $row;
    }
    $output .= theme('table', array(t('Date'), t('Old State'), t('New State'), t('By'), t('Comment')), $rows, array('class' => 'workflow_history'), t('Workflow History'));
    if ($footer_needed) {
      $output .= t('*State is no longer available.');
    }
    print $output . '<br />';
  }
?>
<div id="node-<?php print $node->nid; ?>" class="node <?php print $node_classes; ?>">
  <div class="inner">
    <?php if ($picture || ($title && !$page) || $submitted): ?>
    <div class="clearfix">
      <?php print $picture ?>

      <?php if ($page == 0): ?>
      <h2 class="title"><a href="<?php print $node_url ?>" title="<?php print $title ?>"><?php print $title ?></a></h2>
      <?php endif; ?>

    </div>
    <?php endif; ?>

    <?php if ($node_top && !$teaser): ?>
    <div id="node-top" class="node-top row nested">
      <div id="node-top-inner" class="node-top-inner inner">
        <?php print $node_top; ?>
      </div><!-- /node-top-inner -->
    </div><!-- /node-top -->
    <?php endif; ?>

    <div class="content clearfix">
      <?php print $content ?>
		<?php if ($submitted): ?>
      <div class="meta">
        <span class="submitted"><?php print $submitted ?></span>
      </div>
      <?php endif; ?>	  
		<?php if ($terms): ?>
		<div class="terms">
		  <?php print $terms; ?>
		</div>
		<?php endif;?>
	
		<?php if ($links): ?>
		<div class="links">
		  <?php print $links; ?>
		</div>
		<?php endif; ?>
    </div>

  </div><!-- /inner -->

  <?php if ($node_bottom && !$teaser): ?>
  <div id="node-bottom" class="node-bottom row nested">
    <div id="node-bottom-inner" class="node-bottom-inner inner">
      <?php print $node_bottom; ?>
    </div><!-- /node-bottom-inner -->
  </div><!-- /node-bottom -->
  <?php endif; ?>
<div class="node-shadow pngfix"><img src="<?php print base_path(). path_to_theme() ?>/images/node-shadow.png"  height="19" width="100%"  alt="shadow"/></div>
</div><!-- /node-<?php print $node->nid; ?> -->
