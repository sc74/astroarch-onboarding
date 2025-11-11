#!/bin/zsh

TOKEN_FILE="/usr/share/astroarch_onboarding/configs/astromonitor_token.tmp"
JETON=""

if [ -f "$TOKEN_FILE" ]; then
    JETON=$(cat "$TOKEN_FILE")
fi

if [[ -n "$JETON" ]]; then

	cd /home/astronaut
	astromonitor --retrieve-backup ${JETON}
	EXIT_CODE=$?

	if [[ $EXIT_CODE -ne 0 ]]; then
	notify-send --app-name 'AstroArch' --icon="/home/astronaut/.astroarch/assets/icons/novnc-icon.svg" -t 10000 'Astro Monitor' "❌ Astro Monitor was unable to import your Kstars backup"
	else
	notify-send --app-name 'AstroArch' --icon="/home/astronaut/.astroarch/assets/icons/novnc-icon.svg" -t 10000 'Astro Monitor' "✅ Astro Monitor has successfully imported your Kstars backup"
	fi
fi
