/*$(function(){
	window.setInterval(function(){
		loadnewdata()
	}, 1000)

	function loadnewdata(){
		$.ajax({
			url: "/up_data",
			type: "POST",
			dataType: "json",
			success: function(data){
				$(data_p).replaceWith(data)
			}
		});
	}
});*/
$(function() {
	$('a#anim1').on('click', function(e) {
	  e.preventDefault()
	  $.getJSON('/animation_1',
		  function(data) {
	  });
	  return false;
	});
});
$(function() {
	$('a#anim2').on('click', function(e) {
	  e.preventDefault()
	  $.getJSON('/animation_2',
		  function(data) {
	  });
	  return false;
	});
});
$(function() {
	$('a#anim3').on('click', function(e) {
	  e.preventDefault()
	  $.getJSON('/animation_3',
		  function(data) {
	  });
	  return false;
	});
});
$(function() {
	$('a#brdn').on('click', function(e) {
	  e.preventDefault()
	  $.getJSON('/brightness_down',
		  function(data) {
	  });
	  return false;
	});
});
$(function() {
	$('a#brup').on('click', function(e) {
	  e.preventDefault()
	  $.getJSON('/brightness_up',
		  function(data) {
	  });
	  return false;
	});
});
$(function() {
	$('a#led_off').on('click', function(e) {
	  e.preventDefault()
	  $.getJSON('/led_off',
		  function(data) {
	  });
	  return false;
	});
});