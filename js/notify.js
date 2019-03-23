
function notify($, text) {
    $("#__notify-message").remove();
    let width = 20;
    var notify_space = $("<div></div>")
        .attr('id', '__notify-message')
        .css({
            'text-align': 'center',
            'background-color': 'rgba(68, 68, 68, 0.5)',
            'width': width + 'em',
            'height': '3em',
            'top': '0.1em',
            'left': 'calc(50% - ' + width + 'em/2)',
            'border-radius': '0em 0em 0.5em 0.5em',
            'position': 'fixed',
        });
    notify_space.html('<div style="padding:20px 15px;">' + text + '</div>');
    $('body').prepend(notify_space);
    notify_space.delay(1000).slideUp('slow');
}
