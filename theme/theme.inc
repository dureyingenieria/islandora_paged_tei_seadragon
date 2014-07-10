<?php
/**
 * @file
 * Theme functions for islandora_paged_tei_seadragon.
 */

/**
 * Implements hook_preprocess_theme().
 */
function islandora_paged_tei_seadragon_preprocess_islandora_paged_tei_seadragon_viewer(array &$variables) {
  module_load_include('inc', 'islandora_paged_tei_seadragon', 'includes/utilities');
  module_load_include('inc', 'islandora', 'includes/authtokens');
  $viewer_module_path = drupal_get_path('module', 'islandora_paged_tei_seadragon');
  $manuscript = $variables['object'];
  $transform = $variables['transform_object'];
  if (isset($_GET['islandora_paged_content_page'])) {
    foreach ($variables['pages'] as $page_info) {
      if ($page_info['page'] == $_GET['islandora_paged_content_page']) {
        $page_pid = $page_info['pid'];
        break;
      }
    }
  }
  if (!isset($page_pid)) {
    $page_entry = reset($variables['pages']);
    $page_pid = $page_entry['pid'];
  }

  drupal_add_css("$viewer_module_path/css/viewer.css");
  $variables['tei'] = islandora_paged_tei_seadragon_do_transform($manuscript, $transform);
  if ($page_pid) {
    $token = islandora_get_object_token($page_pid, 'JP2', 2);

    // Not using a render array because it was adding impeding divs.
    $options = array();
    foreach ($variables['pages'] as $pid => $page_info) {
      $options[$pid] = $page_info['page'];
    }
    $pager_variables = array(
      '#options' => $options,
      '#attributes' => array('id' => array('islandora_paged_tei_seadragon_pager')),
      '#value' => $page_pid,
    );
    $variables['pager'] = '<strong>' . t('Image') . ': </strong>' . t('!page_selector of @total', array(
      '!page_selector' => theme('select', array('element' => $pager_variables)),
      '@total' => count($variables['pages']),
    ));

    $variables['clipper'] = theme(
      'islandora_openseadragon_clipper',
      array('pid' => $page_pid)
    );
    $variables['seadragon'] = theme(
      'islandora_openseadragon_viewer',
      array(
        'uri' => url(
          "islandora/object/$page_pid/datastream/JP2/view",
          array('absolute' => TRUE, 'query' => array('token' => $token))
        ),
        'fedora_object' => islandora_object_load($page_pid),
      )
    );
    if (isset($transform['CSS'])) {
      drupal_add_css($transform['CSS']->content, array('type' => 'inline'));
    }
    drupal_add_js("$viewer_module_path/js/update_page.js");
    drupal_add_js("$viewer_module_path/js/tei_toggle.js");
  }
}