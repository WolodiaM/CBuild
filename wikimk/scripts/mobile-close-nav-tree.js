document.addEventListener("DOMContentLoaded", () => {
	if (window.innerWidth <= 900) {
		const nestedNavs = document.querySelectorAll("nav details");
		nestedNavs.forEach(details => {
			details.open = false;
		});
	}
});
