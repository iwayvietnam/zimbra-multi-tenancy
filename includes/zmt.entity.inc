<?php
/**
 * @file
 * Zmt Entity API controller classe.
 */

/**
 * Implements the cache entity controller.
 */
class ZmtEntityController extends EntityDatabaseStorageController {

  /**
   * {@inheritdoc}
   */
  public function resetCache(array $ids = NULL) {
    $this->_resetEntityCache($ids);
    parent::resetCache($ids);
  }

  /**
   * {@inheritdoc}
   */
  public function load($ids = array(), $conditions = array()) {
    return $this->_entityCacheLoad($ids, $conditions);
  }

  private function _resetEntityCache(array $ids = NULL) {
    // Reset the persistent cache.
    if (!empty($ids)) {
      cache_clear_all($ids, 'cache_entity_' . $this->entityType);
    }
    else {
      // Force all cached entries to be deleted.
      cache_clear_all('*', 'cache_entity_' . $this->entityType, TRUE);
    }
  }

  /**
   * Loads entities by IDs/conditions, which are potentially cached.
   *
   * @param array $ids
   *   (optional) entity IDs to load.
   * @param array $conditions
   *   (options) An array of conditions to be used when loading. Note: It is
   *   possible to pass conditions with and without IDs.
   *
   * @return array
   *   An array of loaded entities keyed by ID.
   */
  private function _entityCacheLoad($ids = array(), $conditions = array()) {
    $entities = array();
    $cached_entities = array();
    $queried_entities = array();

    // Revisions are not statically cached, and require a different query to
    // other conditions, so separate the revision id into its own variable.
    if ($this->revisionKey && isset($conditions[$this->revisionKey])) {
      $revision_id = $conditions[$this->revisionKey];
      unset($conditions[$this->revisionKey]);
    }
    else {
      $revision_id = FALSE;
    }

    // Create a new variable which is either a prepared version of the $ids
    // array for later comparison with the entity cache, or FALSE if no $ids
    // were passed. The $ids array is reduced as items are loaded from cache,
    // and we need to know if it's empty for this reason to avoid querying the
    // database when all requested entities are loaded from cache.
    $passed_ids = !empty($ids) ? array_flip($ids) : FALSE;

    // Use an entity field query to transform the list of conditions into
    // the set of entity IDs which the conditions admit, then re-enter this
    // method with that set as the $ids constraint.
    if ($conditions) {
      $query = new EntityFieldQuery();
      $query->entityCondition('entity_type', $this->entityType);
      foreach ($conditions as $property_name => $condition) {
        // Note $condition might be multiple values, which are treated as OR
        // by default.
        $query->propertyCondition($property_name, $condition);
      }

      // Limit the result set also by the passed in IDs.
      if ($passed_ids) {
        $query->propertyCondition($this->idKey, array_keys($passed_ids));
      }

      $result = $query->execute();
      if (isset($result[$this->entityType])) {
        $entity_ids = array_keys($result[$this->entityType]);
        if ($revision_id) {
          return $this->_entityCacheLoad($entity_ids, array($this->revisionKey => $revision_id));
        } else {
          return $this->_entityCacheLoad($entity_ids);
        }
      }
      else {
        // No results found.
        return array();
      }
    }

    // Try to load entities from the static cache, if the entity type supports
    // static caching.
    if ($this->cache && !$revision_id) {
      $entities += $this->cacheGet($ids, $conditions);
      // If any entities were loaded, remove them from the ids still to load.
      if ($passed_ids) {
        $ids = array_keys(array_diff_key($passed_ids, $entities));
      }
    }

    if (!empty($this->entityInfo['entity cache']) && !$revision_id && $ids && !$conditions) {
      $entities += $cached_entities = $this->_entityCacheGet($this, $ids, $conditions);
      // If any entities were loaded, remove them from the ids still to load.
      $ids = array_diff($ids, array_keys($cached_entities));

      if ($this->cache) {
        // Add entities to the cache if we are not loading a revision.
        if (!empty($cached_entities) && !$revision_id) {
          $this->cacheSet($cached_entities);
        }
      }
    }

    // Ensure integer entity IDs are valid.
    if (!empty($ids)) {
      $this->_entityCacheCleanIds($ids);
    }

    // Load any remaining entities from the database. This is the case if $ids
    // is set to FALSE (so we load all entities), if there are any ids left to
    // load, if loading a revision, or if $conditions was passed without $ids.
    if ($ids === FALSE || $ids || $revision_id || ($conditions && !$passed_ids)) {
      // Build the query.
      $query = $this->buildQuery($ids, $conditions, $revision_id);
      $queried_entities = $query
        ->execute()
        ->fetchAllAssoc($this->idKey);
    }

    // Pass all entities loaded from the database through $this->attachLoad(),
    // which attaches fields (if supported by the entity type) and calls the
    // entity type specific load callback, for example hook_node_load().
    if (!empty($queried_entities)) {
      $this->attachLoad($queried_entities, $revision_id);
      $entities += $queried_entities;
    }

    if (!empty($this->entityInfo['entity cache'])) {
      // Add entities to the entity cache if we are not loading a revision.
      if (!empty($queried_entities) && !$revision_id) {
        // Only cache the entities which were loaded by ID. Entities that were
        // loaded based on conditions will never be found via cacheGet() and we
        // would keep on caching them.
        if ($passed_ids) {
          $queried_entities_by_id = array_intersect_key($queried_entities, $passed_ids);
          if (!empty($queried_entities_by_id)) {
            $this->_entityCacheSet($queried_entities_by_id);
          }
        }
      }
    }

    if ($this->cache) {
      // Add entities to the cache if we are not loading a revision.
      if (!empty($queried_entities) && !$revision_id) {
        $this->cacheSet($queried_entities);
      }
    }

    // Ensure that the returned array is ordered the same as the original
    // $ids array if this was passed in and remove any invalid ids.
    if ($passed_ids) {
      // Remove any invalid ids from the array.
      $passed_ids = array_intersect_key($passed_ids, $entities);
      foreach ($entities as $entity) {
        $passed_ids[$entity->{$this->idKey}] = $entity;
      }
      $entities = $passed_ids;
    }

    return $entities;
  }

  private function _entityCacheCleanIds(&$ids) {
    $entity_info = entity_get_info($this->entityType);
    if (isset($entity_info['base table field types'])) {
      $id_type = $entity_info['base table field types'][$this->idKey];
      if ($id_type == 'serial' || $id_type == 'int') {
        $ids = array_filter($ids, array(__CLASS__, 'entityCacheFilterId'));
        $ids = array_map('intval', $ids);
      }
    }
  }

  private function _entityCacheGet($ids, $conditions = array()) {
    $cached_entities = array();
    if ($ids && !$conditions) {
      $cached = cache_get_multiple($ids, 'cache_entity_' . $this->entityType);
      if ($cached) {
        foreach ($cached as $item) {
          $cached_entities[$item->cid] = $item->data;
        }
      }
    }
    return $cached_entities;
  }

  private function _entityCacheSet(array $entities = array()) {
    foreach ($entities as $id => $item) {
      cache_set($id, $item, 'cache_entity_' . $this->entityType);
    }
  }
}
