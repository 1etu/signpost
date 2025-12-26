# signpost

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/platform-linux%20%7C%20macos-lightgrey.svg)]()
[![Minecraft](https://img.shields.io/badge/minecraft-1.8.9--1.21.x-green.svg)]()

Lightweight Minecraft server for domain protection. Displays redirect messages on parked and look-alike domains.

---

## Overview

When players add your defensive domain to their server list, they see your official address. When they try to join, they're disconnected with a redirect message.

Large networks register defensive domains — typos, old names, similar TLDs — to prevent phishing or player confusion. **signpost** runs on these domains to guide players to the correct address.

```
┌─────────────────┐      ┌──────────────┐      ┌─────────────────┐
│  Player adds    │ ──── │   signpost   │ ──── │  "Connect to    │
│  typo-domain    │ DNS  │   responds   │      │  play.real.net" │
└─────────────────┘      └──────────────┘      └─────────────────┘
```

One binary. Unlimited domains. No runtime dependencies.

---

## Quick Start

```bash
# Build
make

# Configure
vim config.json

# Run
./signpost
```

---

## Installation

### From Source

Requires C++17 compatible compiler (GCC 8+, Clang 7+, Apple Clang 10+).

```bash
git clone https://github.com/yourname/signpost.git
cd signpost
make
```

### CMake

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
sudo make install  # optional, installs to /usr/local/bin
```

---

## Configuration

Create `config.json` in the working directory:

```json
{
    "host": "0.0.0.0",
    "port": 25565,

    "motd": "§7§oPlease connect via §6§oplay.example.net",
    "motd_outdated": "§7§oServer requires §c§o1.20.4+",
    "min_protocol": 765,

    "kick_message": "§7§oPlease connect via §6§oplay.example.net",

    "version_name": "Example Network",
    "max_players": 0,
    "online_players": 0,

    "favicon": "server-icon.png"
}
```

Or specify a path:

```bash
./signpost /etc/signpost/config.json
```

### Options

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `host` | string | `0.0.0.0` | Bind address |
| `port` | int | `25565` | Listen port |
| `motd` | string | required | Server list description |
| `motd_outdated` | string | `""` | Appended when client is outdated |
| `min_protocol` | int | `0` | Minimum protocol version (0 = disabled) |
| `kick_message` | string | required | Disconnect message on join |
| `version_name` | string | `""` | Version text in server list |
| `max_players` | int | `0` | Displayed max player count |
| `online_players` | int | `0` | Displayed online player count |
| `favicon` | string | `""` | Path to 64x64 PNG server icon |

### Server Icon

Display your network's branding in the server list. The icon must be a 64x64 PNG file.

```json
{
    "favicon": "server-icon.png"
}
```

Use the same icon as your main server for brand consistency across all your domains.

### Version-Aware Messaging

If your main server requires a minimum Minecraft version, you can warn outdated players before they even try to connect. This saves them time — they'll know to update their client before joining the real server.

```json
{
    "motd": "§6play.example.net",
    "motd_outdated": "§cUpdate to 1.20.4+ before joining!",
    "min_protocol": 765
}
```

- Players on **1.20.4+** see: `play.example.net`  
- Players on **1.20.3 or below** see both lines, warning them to update first

This way, players arrive at your official server with the right version already.

## Deployment

### DNS Setup

Point all defensive domains to your signpost server:

```
typo-example.net    A    203.0.113.10
exampel.net         A    203.0.113.10  
example-mc.com      A    203.0.113.10
old-name.net        A    203.0.113.10
```

### systemd

```ini
# /etc/systemd/system/signpost.service
[Unit]
Description=signpost Minecraft redirect server
After=network.target

[Service]
Type=simple
ExecStart=/usr/local/bin/signpost /etc/signpost/config.json
Restart=always
RestartSec=5
User=nobody
Group=nogroup

[Install]
WantedBy=multi-user.target
```

```bash
sudo systemctl enable signpost
sudo systemctl start signpost
```

### Docker

```dockerfile
FROM alpine:latest AS builder
RUN apk add --no-cache build-base
WORKDIR /src
COPY . .
RUN make

FROM alpine:latest
COPY --from=builder /src/signpost /usr/local/bin/
COPY config.json /etc/signpost/
EXPOSE 25565
CMD ["signpost", "/etc/signpost/config.json"]
```

```bash
docker build -t signpost .
docker run -d -p 25565:25565 -v ./config.json:/etc/signpost/config.json signpost
```

---

## How It Works

signpost implements the minimum Minecraft protocol surface required for server list ping and login rejection.

### Protocol States

```
┌─────────────┐    Handshake     ┌────────────┐
│ Handshaking │ ───────────────► │   Status   │ ──► Server list ping
└─────────────┘     next=1       └────────────┘
       │
       │ next=2      ┌────────────┐
       └───────────► │   Login    │ ──► Disconnect with message
                     └────────────┘
```

### Packets Handled

| State | Packet | Action |
|-------|--------|--------|
| Handshaking | `0x00` Handshake | Parse version, transition state |
| Status | `0x00` Status Request | Respond with JSON |
| Status | `0x01` Ping | Echo payload as Pong |
| Login | `0x00` Login Start | Send Disconnect |

No encryption, compression, or play state. Connections are stateless and short-lived.

---

## Performance

signpost is designed for minimal resource usage:

- **Memory**: < 1MB resident
- **CPU**: Negligible (< 0.1% idle)
- **Connections**: Handles thousands of concurrent pings
- **Binary size**: ~60KB stripped

No threads, no allocations in hot path, no external dependencies.

---

## Contributing

Contributions welcome. Please keep the codebase minimal and focused.

1. Fork the repository
2. Create a feature branch
3. Submit a pull request

---

## License

MIT License. See [LICENSE](LICENSE) for details.

---

<p align="center">
<sub>Built for networks that care about their players finding the right door.</sub>
</p>
