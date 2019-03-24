
function notify($, text) {
    let is_shown = $("#__notify-message").css('display') == 'block';
    $("#__notify-message").remove();
    let width = 20;
    var notify_space = $("<div></div>")
        .attr('id', '__notify-message')
        .css({
            'text-align': 'center',
            'background-color': 'rgba(68, 68, 68, 0.5)',
            'width': width + 'em',
            'height': '3em',
            'display': (is_shown) ? 'block': 'none',
            'top': '0.3em',
            'left': 'calc(50% - ' + width + 'em/2)',
            'border-radius': '0.5em 0.5em 0.5em 0.5em',
            'position': 'fixed',
        });
    notify_space.html('<div style="padding:20px 15px;">' + text + '</div>');
    $('body').prepend(notify_space);
    if(!is_shown)
    {
        notify_space.fadeIn('slow');
    }
    notify_space.delay(1000).fadeOut('slow');
}
