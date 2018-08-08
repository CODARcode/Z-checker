(function initialConnectDialog() {
  var clickConnect = function() {
    var ip = $('#hostAddr').val();
    var port = $('#port').val();
    Client.connectToServer(ip, port);
    $('#connectDialog').modal('hide');
    $('#loading').show();
  };
  $('#connect').click(function() {
    clickConnect();
  });

  connectionDialog = {};

  connectionDialog.error = function() {
    $('#loading').hide();
    $('#connect-dialog-title')[0].innerHTML = 'Connection Error. Try Again';
    $('#close-dialog').hide();
    $('#close-dialog-cross').hide();
    $('#connectDialog').modal('show');
  };
  connectionDialog.closed = function() {
    $('#loading').hide();
    $('#connect-dialog-title')[0].innerHTML = 'Connection Closed. Try Again';
    $('#close-dialog').hide();
    $('#close-dialog-cross').hide();
    $('#connectDialog').modal('show');
  }
  connectionDialog.reconnect = function() {
    $('#loading').hide();
    $('#connect-dialog-title')[0].innerHTML = 'Reconnect to Server';
    $('#close-dialog').show();
    $('#close-dialog-cross').show();
    $('#connectDialog').modal('show');
  }

  document.querySelector('#connectDialog').addEventListener('keypress', function (e) {
      var key = e.which || e.keyCode;
      if (key === 13) { // 13 is enter
        console.log('connect');
        clickConnect();
      }
  });

  // if (!DEBUG_MODE) $('#connectDialog').modal('show');
  $('#connectDialog').modal('show');
})();

$(document).ready(function() {
  // $(".my-container").draggable();
  // $(".my-container").resizable();
});
