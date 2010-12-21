<?php
// $Id$

/**
 * @file
 *   Feeds parser class for Slideshare.
 *
 * Inspired by Flickr parser by dragonwize
 */

/**
 * Class definition for Slideshare SimplePie Parser.
 *
 * Parses RSS and Atom feeds.
 */
class FeedsSlideshareSimplePieParser extends FeedsSimplePieParser {

  /**
   * Parse the extra mapping sources provided by this parser.
   */
  protected function parseExtensions(&$item, $simplepie_item) {
    // Extract media fields out of the enclosure.
    $enclosure = $simplepie_item->get_enclosure();
    if (empty($enclosure) || !is_object($enclosure)) {
      return;
    }

    $item['media_link']     = $enclosure->get_link();
    $item['media_width']    = $enclosure->get_width();
    $item['media_height']   = $enclosure->get_height();
    $item['media_title']    = $enclosure->get_title();
    $item['media_desc']     = $enclosure->get_description();
    $item['media_credit']   = $enclosure->get_credit() ? $enclosure->get_credit()->get_name() : '';
    $item['media_category'] = $enclosure->get_category() ? $enclosure->get_category()->get_term() : '';
    $item['media_thumbnail'] = $enclosure->get_thumbnail();
  }

  /**
   * Add the extra mapping sources provided by this parser.
   */
  public function getMappingSources() {
    return parent::getMappingSources() + array(
      'media_title' => array(
        'name' => t('Media: Title'),
        'description' => t('Media:title field.'),
      ),
      'media_desc' => array(
        'name' => t('Media: Description'),
        'description' => t('Media:description field.'),
      ),
      'media_credit' => array(
        'name' => t('Media: Credit'),
        'description' => t('Media:credit field.'),
      ),
      'media_category' => array(
        'name' => t('Media: Category'),
        'description' => t('Media:category field.'),
      ),
      'media_link' => array(
        'name' => t('Media: Link'),
        'description' => t('Media:content url field.'),
      ),
      'media_width' => array(
        'name' => t('Media: Width'),
        'description' => t('Media:content width field.'),
      ),
      'media_height' => array(
        'name' => t('Media: Height'),
        'description' => t('Media:content height field.'),
      ),
      'media_thumbnail_75x75' => array(
        'name' => t('Media: Thumbnail'),
        'description' => t('Media:thumbnail field.'),
      ),
     );
  }

}
