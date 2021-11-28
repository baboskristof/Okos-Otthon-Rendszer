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

const gaugeElement = document.querySelector(".gauge");

function setGaugeValue(gauge, value) {
  if (value < 0 || value > 1) {
    return;
  }

  gauge.querySelector(".gauge-fill").style.transform = `rotate(${
    value / 2
  }turn)`;
  gauge.querySelector(".gauge-cover").textContent = `${Math.round(
    value * 100
  )}%`;
}
setGaugeValue(gaugeElement, 0.3);
