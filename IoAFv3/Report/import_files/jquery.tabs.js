/* jquery.tabs.js - jQuery 탭 플러그인, 2012 © yamoo9.com

	옵션
	start_index: 시작 인덱스 설정,
	random: 랜덤 인덱스 설정,
	transition_time: 장면 전환 시간 설정 (밀리초, 1/1000초)
---------------------------------------------------------------- */
;(function($) { 
	$.fn.tabs = function(options) {
		
		// 옵션 설정.
		options = $.extend({
			start_index: 1,
			random: false,
			transitions_time: 400
		}, options);
		
		// jQuery 체인.
		return this.each(function() {
			
			// 대상 참조.
			var $this = $(this),
				$menu = $this.find('.tab_menu'),
				$menu2 = $menu.find('#1'),
				$menu_li2 = $menu2.find('li'),
				$menu_a2 = $menu_li2.find('a'),
				$contents2 = $this.find('.tab_contents').find('#1'),
				$menu3 = $menu.find("#2"),
				$menu_li3 = $menu3.find('li'),
				$menu_a3 = $menu_li3.find('a'),
				$contents3 = $this.find('.tab_contents').find('#2'),
				support_features = !Modernizr.opacity || !Modernizr.csstransitions;
			


			// 플러그인 대상 객체에 클래스 부여	
			$this.addClass('tabs');
			
			// 처음 보여질 탭 설정.
			$menu2.add($contents2)
				.find('li:nth-child(1)').addClass('active');

			$menu3.add($contents3)
				.find('li:nth-child(1)').addClass('active');
			
			// opacity 미지원 브라우저에 적용.
			if(support_features) {
				
				$menu_li2.find('img').animate({'opacity': 0}, 10, function() {
					$menu_li2.filter('.active').find('img').animate({'opacity': 1}, 10);	
				});

				$menu_li3.find('img').animate({'opacity': 0}, 10, function() {
					$menu_li3.filter('.active').find('img').animate({'opacity': 1}, 10);	
				});
				
				$menu_a2
					.mouseover(function() {
						$(this)
							.stop().animate({'padding-left': '2.2em', 'padding-right': '0.8em'}, 200)
							.find('img').stop().animate({'opacity': 1, 'left': 6}, 200);
					})
					.mouseout(function() {
						if($(this).parent().hasClass('active')) return false;
						$(this)
							.stop().animate({'padding-left': '1.5em', 'padding-right': '1.5em'}, 200)
							.find('img').stop().animate({'opacity':0, 'left': 16}, 200);
					});

				$menu_a3
					.mouseover(function() {
						$(this)
							.stop().animate({'padding-left': '2.2em', 'padding-right': '0.8em'}, 200)
							.find('img').stop().animate({'opacity': 1, 'left': 6}, 200);
					})
					.mouseout(function() {
						if($(this).parent().hasClass('active')) return false;
						$(this)
							.stop().animate({'padding-left': '1.5em', 'padding-right': '1.5em'}, 200)
							.find('img').stop().animate({'opacity':0, 'left': 16}, 200);
					});
			};
				
			// $menu 내부의 a 클릭 시 이벤트 핸들링
			$menu_a2.click(function(e) {
			
				// 대상 참조.
				var $this = $(this),
					target = $this.attr('href');
					
				// 활성화된 a 클릭 시, 작동하지 않도록 설정.
	 			if($this.parent().hasClass('active')) return;
	 			
	 			// $menu_link에서 active 클래스 제거
				$menu_li2.removeClass('active');
				
				// 클릭한 a의 부모 li에 active 클래스 추가
				$this.parent().addClass('active');
				
				// opacity 미지원 브라우저에 적용.	
				if(support_features) {
					$menu_li2.not('.active').find('a').mouseout();
					$(this).mouseover();
				};
				
				// 부드러운 장면전환.
				$contents2.find('li')
					.fadeTo(options.transition_time, 0, function() {
						$(this).removeClass('active')
							.filter(target).addClass('active').fadeTo(options.transition_time, 1);
				});
			
				// 브라우저 링크 기본 동작 차단
				e.preventDefault();
				
			});

			$menu_a3.click(function(e) {
			
				// 대상 참조.
				var $this = $(this),
					target = $this.attr('href');
					
				// 활성화된 a 클릭 시, 작동하지 않도록 설정.
	 			if($this.parent().hasClass('active')) return;
	 			
	 			// $menu_link에서 active 클래스 제거
				$menu_li3.removeClass('active');
				
				// 클릭한 a의 부모 li에 active 클래스 추가
				$this.parent().addClass('active');
				
				// opacity 미지원 브라우저에 적용.	
				if(support_features) {
					$menu_li3.not('.active').find('a').mouseout();
					$(this).mouseover();
				};
				
				// 부드러운 장면전환.
				$contents3.find('li')
					.fadeTo(options.transition_time, 0, function() {
						$(this).removeClass('active')
							.filter(target).addClass('active').fadeTo(options.transition_time, 1);
				});
			
				// 브라우저 링크 기본 동작 차단
				e.preventDefault();
				
			});
			
		}); // end: return
	}; //end: plug-in
})(jQuery);










