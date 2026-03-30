const THEME_BUTTON_ID="theme-switch";
const THEME_STORAGE_ID="theme";
const THEME_CSS_ID="data-theme";
const	THEME_DARK="dark";
const THEME_LIGHT="light";
const THEME_SYSTEM="system";
const THEME_SYSTEM_THEME_EXTRACTOR="(prefers-color-scheme: dark)";

function setTheme(t) { // t can be "dark" or "light"
	document.documentElement.setAttribute(THEME_CSS_ID, t);
}
function systemThemeEvent(dark) {
	const storedTheme = localStorage.getItem(THEME_STORAGE_ID);
	if (storedTheme === null || storedTheme === THEME_SYSTEM) {
		const newTheme = dark.matches ? THEME_DARK : THEME_LIGHT;
		setTheme(newTheme);
	}
}
function toggleTheme() {
	const storedTheme = localStorage.getItem(THEME_STORAGE_ID);
	if (storedTheme == null) {
		return setupTheme()
	}
	switch (storedTheme) {
	case THEME_SYSTEM:
		localStorage.setItem(THEME_STORAGE_ID, THEME_DARK);
		setTheme(THEME_DARK);
		break;
	case THEME_DARK:
		localStorage.setItem(THEME_STORAGE_ID, THEME_LIGHT);
		setTheme(THEME_LIGHT);
		break;
	case THEME_LIGHT:
		localStorage.setItem(THEME_STORAGE_ID, THEME_SYSTEM);
		const systemTheme = window.matchMedia(THEME_SYSTEM_THEME_EXTRACTOR);
		systemThemeEvent(systemTheme);
		break;
	}
}
function setupTheme() {
	const storedTheme = localStorage.getItem(THEME_STORAGE_ID);
	if (storedTheme === THEME_DARK || storedTheme === THEME_LIGHT) {
		setTheme(storedTheme);
	} else if (storedTheme === THEME_SYSTEM) {
		const systemTheme = window.matchMedia(THEME_SYSTEM_THEME_EXTRACTOR);
		systemThemeEvent(systemTheme);
	} else {
		localStorage.setItem(THEME_STORAGE_ID, THEME_SYSTEM);
		const systemTheme = window.matchMedia(THEME_SYSTEM_THEME_EXTRACTOR);
		systemThemeEvent(systemTheme);
	}
}
function switchButtonThemeText() {
	const storedTheme = localStorage.getItem(THEME_STORAGE_ID);
	const button = document.getElementById(THEME_BUTTON_ID);
	switch (storedTheme) {
	case THEME_SYSTEM:
		button.innerHTML="System theme";
		break;
	case THEME_DARK:
		button.innerHTML="Dark theme";
		break;
	case THEME_LIGHT:
		button.innerHTML="Light theme";
		break;
	}
}
function loadPageTheme() {
	// Click handler
	const button = document.getElementById(THEME_BUTTON_ID);
	button.addEventListener("click", toggleTheme);
	button.addEventListener("click", switchButtonThemeText);
	// Setup handler for a system theme
	const systemTheme = window.matchMedia(THEME_SYSTEM_THEME_EXTRACTOR);
	systemTheme.addEventListener("change", systemThemeEvent);
	// Setup default theme - system default
	switchButtonThemeText();
}
setupTheme();
document.addEventListener("DOMContentLoaded", () => {
	loadPageTheme();
});
