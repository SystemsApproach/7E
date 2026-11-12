(function () {
  'use strict';

  var storageKey = '7e-sidebar-collapsed';
  var root = document.documentElement;

  function initializeSidebarToggle() {
    var toggle = document.querySelector('.desktop-sidebar-toggle');
    var sidebar = document.querySelector('.wy-nav-side');

    if (!toggle || !sidebar) {
      return;
    }

    if (!sidebar.id) {
      sidebar.id = 'wy-nav-side';
    }
    toggle.setAttribute('aria-controls', sidebar.id);

    function updateToggle() {
      var expanded = !root.classList.contains('sidebar-collapsed');
      var label = expanded ? 'Hide navigation sidebar' : 'Show navigation sidebar';
      toggle.setAttribute('aria-expanded', String(expanded));
      toggle.setAttribute('aria-label', label);
      toggle.setAttribute('title', label);
    }

    updateToggle();

    toggle.addEventListener('click', function () {
      var collapsed = root.classList.toggle('sidebar-collapsed');
      updateToggle();

      try {
        window.localStorage.setItem(storageKey, String(collapsed));
      } catch (error) {
        // The current page still works if browser storage is unavailable.
      }
    });
  }

  if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', initializeSidebarToggle, { once: true });
  } else {
    initializeSidebarToggle();
  }
})();
