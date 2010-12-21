<?php
// $Id$

/**
 * @file
 * Feeds parser class for Slideshare
 */

/**
 * Class definition for Slideshare Parser.
 *
 * Parses RSS and Atom feeds.
 */
class FeedsSlideshareParser extends FeedsParser {

  /**
   * Parse the extra mapping sources provided by this parser.
   */
  public function parse(FeedsImportBatch $batch, FeedsSource $source) {

    $slideshare_feed = $batch->getRaw();

    /** @see common_syndication_parser_parse() */
    if (!defined('LIBXML_VERSION') || (version_compare(phpversion(), '5.1.0', '<'))) {
      @ $sxml = simplexml_load_string($slideshare_feed, NULL);
    }
    else {
      @ $sxml = simplexml_load_string($slideshare_feed, NULL, LIBXML_NOERROR | LIBXML_NOWARNING | LIBXML_NOCDATA);
    }

    // Got a malformed XML.
    if ($sxml === FALSE || is_null($sxml)) {
      // Last chance to repair XML source - replace UTF8 errors
      // @todo replace this when Slideshare Support fix the issue (?)
      $slideshare_feed = $this->repairXmlUtf8Errors($slideshare_feed);
      @ $sxml = simplexml_load_string($slideshare_feed, NULL);

      if ($sxml === FALSE || is_null($sxml)) {
        watchdog('slideshare_parser', 'Parsing of the Slideshare feed @url failed. Provided link isn\'t valid XML source.', array('@url' => $batch->link), WATCHDOG_ERROR);
        return;
      }
      else {
        // Just warn site administrators that XML feed wasn't 100% OK
        watchdog('slideshare_parser', 'Slideshare feed @url contains invalid UTF-8 characters.', array('@url' => $batch->link), WATCHDOG_WARNING);
      }
    }

    // XML was parsed successfully, so we can begin to process items
    $batch->title = (string) $sxml->channel->title;
    $batch->description = (string) $sxml->channel->description;
    $batch->link = (string) $sxml->channel->link;

    // Iterate over entries in feed
    foreach ($sxml->xpath('//item') as $entry) {

      // get nodes in atom
      $atom = $entry->children('http://www.w3.org/2005/Atom');

      // get nodes in media: namespace for media information
      $media = $entry->children('http://search.yahoo.com/mrss/');

      // get video player URL
      $attrs = $media->content->player->attributes();
      $embed = $attrs['url'];

      // get video ID
      $id = $guid = $entry->guid;

      // get <slideshare> node
      $slideshare = $entry->children('http://slideshare.net/api/1');
      if (preg_match('/id="__ss_(\d+)"/i', $slideshare->embed, $matches)) {
        // Better GUID - make sure we don't interfere with other services
        $id = $matches[1];
        $guid = 'slideshare:' . $id;
      }

      $item = array(
        'guid' => $guid,
        'slideshare_id' => $id,
        'url' => (string) $entry->link,
        'title' => html_entity_decode((string) $media->content->title),
        'timestamp' => strtotime($entry->pubDate),
        'author_name' => (string) $entry->author,
        'description' => html_entity_decode((string) $media->content->description),
        'thumbnail' => (string) $slideshare->meta->thumbnail,
        'slideshare_type' => (string) $slideshare->meta->type,
        'tags' => explode(',', $media->content->keywords),
        'embedded_player' => (string) $embed,
        'views_count' => (int) $slideshare->meta->views,
        'comments_count' => (int) $slideshare->meta->comments,
      );

      // Populate the FeedsImportBatch object with the parsed results.
      $batch->items[] = $item;
    }

//    dpm($item, 'item');
//    dpm($entry, 'entry');
//    dpm($atom, 'atom');
//    dpm($media, 'media');
  }

  /**
   * Add the extra mapping sources provided by this parser.
   */
  public function getMappingSources() {
    return parent::getMappingSources() + array(
      'guid' => array(
        'name' => t('Item GUID'),
        'description' => t('Global Unique Identifier of the feed item.'),
      ),
      'url' => array(
        'name' => t('Item URL (link)'),
        'description' => t('URL of the feed item.'),
      ),
      'title' => array(
        'name' => t('Video title'),
        'description' => t('Video title.'),
      ),
      'timestamp' => array(
        'name' => t('Published date'),
        'description' => t('Published date as UNIX time GMT of the slideshare item.'),
      ),
      'author_name' => array(
        'name' => t('Author name'),
        'description' => t('Name of the slideshare item\'s author.'),
      ),
      'description' => array(
        'name' => t('Description'),
        'description' => t('Description of the slideshare item.'),
      ),
      'tags' => array(
        'name' => t('Categories'),
        'description' => t('An array of categories that have been assigned to the feed item.'),
      ),
      'slideshare_id' => array(
        'name' => t('Slideshare ID'),
        'description' => t('Slideshare item\'s unique ID.'),
      ),
      'slideshare_type' => array(
        'name' => t('Slideshare type'),
        'description' => t('Type of the Slideshare document.'),
      ),
      'thumbnail' => array(
        'name' => t('Thumbnail'),
        'description' => t('Slideshare thumbnail field.'),
      ),
      'embedded_player' => array(
        'name' => t('Embedded player'),
        'description' => t('Slideshare embedded player.'),
      ),
      'views_count' => array(
        'name' => t('View count'),
        'description' => t('Slideshare document views count.'),
      ),
      'comments_count' => array(
        'name' => t('Comments count'),
        'description' => t('Slideshare document comments count.'),
      ),
    );
  }

  /**
   * Replace invalid UTF-8 characters from XML source
   *
   * @param string $source
   */
  protected function repairXmlUtf8Errors($source) {
    return preg_replace('/[^\x9\xA\xD\x20-\x{D7FF}\x{E000}-\x{FFFD}\x{10000}-\x{10FFFF}]/u','', $source);
  }

}
