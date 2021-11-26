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
	$('a#brdn').on('click', function (e) {
		e.preventDefault()
		$.getJSON('/brightness_down',
			function (data) {
			});
		return false;
	});
});
$(function() {
	$('a#brup').on('click', function (e) {
		e.preventDefault()
		$.getJSON('/brightness_up',
			function (data) {
			});
		return false;
	});
});
$(function() {
	$('a#led_off').on('click', function (e) {
		e.preventDefault()
		$.getJSON('/led_off',
			function (data) {
			});
		return false;
	});
});
$(function() {
	$('#cardSw1').click(function () {
		if ($(this).is(':checked')) {
			$.getJSON('/sw1_on',
				function () {
				});
		} else {
			$.getJSON('/sw1_off',
				function () {
				});
		}
	});
});
$(function() {
	$('#cardSw2').click(function () {
		if ($(this).is(':checked')) {
			$.getJSON('/sw2_on',
				function (data) {
				});
		} else {
			$.getJSON('/sw2_off',
				function (data) {
				});
		}
	});
});