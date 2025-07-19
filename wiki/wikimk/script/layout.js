document.addEventListener("DOMContentLoaded", () => {
	const nav = document.querySelector("nav details.nav-toggle");
	if (!nav) return;
	nav.open = window.innerWidth >= 769;
});
