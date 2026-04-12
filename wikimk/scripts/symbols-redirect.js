window.addEventListener("DOMContentLoaded", () => {
	const hash = window.location.hash.substring(1);
	if (hash) {
		const entry = document.getElementById(hash);
		if (entry) {
			const link = entry.querySelector('a');
			if (link && link.href) {
				window.location.replace(link.href);
			}
		}
	}
});
