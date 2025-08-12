// script.js
document.addEventListener('DOMContentLoaded', function () {
	const animateOnScroll = () => {
		const elements = document.querySelectorAll('.feature-card, .team-card');
		const windowHeight = window.innerHeight;

		elements.forEach(element => {
			const elementTop = element.getBoundingClientRect().top;
			const elementVisible = 150;

			if (elementTop < windowHeight - elementVisible) {
				element.style.opacity = '1';
				element.style.transform = 'translateY(0)';
			}
		});
	};

	const initElements = () => {
		const featureCards = document.querySelectorAll('.feature-card');
		const teamCards = document.querySelectorAll('.team-card');

		featureCards.forEach(card => {
			card.style.opacity = '0';
			card.style.transform = 'translateY(30px)';
			card.style.transition = 'opacity 0.6s ease, transform 0.6s ease';
		});

		teamCards.forEach(card => {
			card.style.opacity = '0';
			card.style.transform = 'translateY(30px)';
			card.style.transition = 'opacity 0.6s ease, transform 0.6s ease';
		});
	};

	const initButtons = () => {
		const buttons = document.querySelectorAll('.btn, .team-link, .github-button');

		buttons.forEach(button => {
			button.addEventListener('mouseenter', function () {
				this.style.transform = 'translateY(-5px)';
			});

			button.addEventListener('mouseleave', function () {
				this.style.transform = 'translateY(0)';
			});
		});
	};

	initElements();
	initButtons();
	animateOnScroll();

	window.addEventListener('scroll', animateOnScroll);

	const techItems = document.querySelectorAll('.tech-item');
	techItems.forEach(item => {
		item.addEventListener('mouseenter', () => {
			item.style.transform = 'translateY(-5px)';
		});

		item.addEventListener('mouseleave', () => {
			item.style.transform = 'translateY(0)';
		});
	});
});

const uploadBtn = document.getElementById('uploadBtn');
	const fileInput = document.getElementById('fileInput');
	const form = document.getElementById('uploadForm');

	uploadBtn.addEventListener('click', () => {
		fileInput.click();
	});

	fileInput.addEventListener('change', () => {
		if(fileInput.files.length > 0){
		const file = fileInput.files[0];
		// Build the action URL dynamically: upload/filename
		form.action = `http://localhost:4000/upload/${encodeURIComponent(file.name)}`;
		form.submit();
		}
});
