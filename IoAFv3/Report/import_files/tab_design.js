/* tab_design.js - 탭 디자인, 2012 © yamoo9.com
---------------------------------------------------------------- */
jQuery(function($) {

	$('#tab_design').tabs({
		start_index: 2,
		random: true,
		transition_time: 200
	});
	
	/* ///////////////////////////////////////////////////
		IE 9 이하 브라우저를 위한 PIE 라이브러리 활용.
		border-radius | box-shadow | linear-gradient
	/////////////////////////////////////////////////// */			
	if($.browser.msie && $.browser.version < 10) {
		$.getScript('include/js/libs/PIE.min.js', function() {
			var target = $.browser.version <= 6 ? '.tab_contents' : '.tab_menu a, .tab_contents';
			$(target).each(function() { PIE.attach(this); });
		});
	};
	
});

