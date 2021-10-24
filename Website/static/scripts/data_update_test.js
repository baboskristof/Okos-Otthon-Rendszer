
	$(function(){
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
	});


