!function ($) {

  $(function(){

    /* UTILITIES - added by Aaron
    -----------------------------------*/

    /*Enables en masse all Popovers on <i>icon and <a>link elements set up like so:
    data-toggle="popover"
    data-placement="top"
    data-content="Vivamus sagittis lacus vel augue laoreet rutrum faucibus."
    data-original-title="Popover on top"*/
    $('i[data-toggle=popover],a[data-toggle=popover]').popover({trigger: 'click',container: 'body'})

    // Placeholder support
    $('input, textarea').placeholder()

    // Disable most "#" links en masse
    $('section [href^=#]').click(function(event) {
      event.preventDefault()
    })

    // Disable most focus outlines en masse
    $('a,button,input').each(function() {
      $(this).attr('hideFocus', 'true').css('outline', 'none')
    })

    // WIZARD - Tab header items width fallback
    // Useful for x cases:
    // - `data-steps` was neglected to be set on `.nav-wizard`
    // - `data-steps` was set incorrectly on `.nav-wizard`
    // - the `data-steps` value, though perhaps correct, exceeds the current 10-step maximum provided for by CSS classes.
    $('.nav-wizard').each(function() {
      var $wizSteps = $(this).children()
      if ($wizSteps.length === $(this).data('steps') && $wizSteps.length <= 10) return 
      var wizStepWidth = 100 / $wizSteps.length - 0.1
      $wizSteps.css('width', wizStepWidth + '%')
    })

    // WIZARD - don't show `previous` on first step, don't show `next` on last step
    function showPager() {
      $(".wizard").each(function(){
        if ($(this).children('.nav-wizard').children('li:first-child').hasClass('active')) {
          $(this).children('.pager').children('.previous').hide()
        } else {
          $(this).children('.pager').children('.previous').show()
        }
        if ($(this).children('.nav-wizard').children('li:last-child').hasClass('active')) {
          $(this).children('.pager').children('.next').hide()
        } else {
          $(this).children('.pager').children('.next').show()
        }
      });
    }

    // Setup Pager on document.ready
    showPager()

    // WIZARD - Tab/Pager coordination
    // We want Tab to update when Pager is clicked,
    // and Pager to hide/show buttons approriately when header is clicked,
    // Use the $().tab `shown` event to set Pager (see Tabs documentation)
    $('a[data-toggle="tab"]').on('shown', function(e) {
      showPager()
    })

    // When a mouseclick occurs on a Pager, execute `.click()` on the appropriate Tab.
    $('.wizard .pager a').click(function(e) {
      e.preventDefault()
      // do nothing if there is no nav-wizard to coordinate with...
      if ($(this).closest('.wizard').children('.nav-wizard').length === 0) return
      var theParent   = $(this).parent('li')
      ,   currentStep = $(this).closest('.wizard').children('.nav-wizard').children('.active')
      // limit to `next` and `prev`...
      if (!theParent.hasClass('next') && !theParent.hasClass('previous')) return
      theParent.hasClass('next') ? currentStep.next('li').children('a').click() : currentStep.prev('li').children('a').click()
    })

    // Loading Block
    function loadingBlock(action) {
      var ui = '<div id="loading-block"><div class="modal-backdrop fade in"></div><div class="loading-indicator"></div></div>'
      action !== 'stop' ? $(ui).appendTo('body') : $('#loading-block').remove()
    }

    /* APPLICATION
    -----------------------------------*/
    // just demos - delete them!

    $('#MySpinner').spinner();

    $('#datepicker').datepicker();

    $('#loadingBlockDemo').click(function() {
      loadingBlock();
      setTimeout(function() {loadingBlock('stop')}, 5000);
    });
    $('body').on('click','.loading-indicator',function() {
      loadingBlock('stop');
    });
    
    $(".toggleBtn").on('click',function(){
        $(this).children('.icon-minus, .icon-plus').toggleClass("icon-minus icon-plus");
    });

  })
}(window.jQuery)