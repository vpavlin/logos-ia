# logos-ia — Internet Archive Client Module

C++ backend module for the Logos platform providing search, download, and bookmark management for Internet Archive collections.

## Features

- **Search**: Advanced search against archive.org with mediatype and collection filters
- **Item Metadata**: Retrieve detailed metadata for any IA item
- **Qt Remote Objects IPC**: Async communication between backend and QML frontend via `.rep` interface
- **Integration Tests**: Live API tests against archive.org

## Architecture

```
┌─────────────────┐     Qt Remote Objects      ┌──────────────────┐
│  ia-client-ui   │ ◄──── IaBackendReplica ──► │   logos-ia       │
│  (QML frontend) │                            │   (C++ backend)  │
└─────────────────┘                            └──────────────────┘
                                                   │
                                              QNetworkAccessManager
                                                   │
                                              archive.org API
```

### Key Components

- **SearchClient** (`src/search_client.cpp`): Handles HTTP requests to archive.org/advancedsearch.php and /metadata endpoints
- **IaBackend** (`src/ia_backend.rep`): Qt Remote Objects interface definition for IPC
- **ia_plugin** (`src/ia_plugin.cpp`): Logos module plugin entry point

## Building

```bash
# Build the module library
nix build .#default

# Build LGX bundle for Basecamp loading
nix build .#lgx           # Dev variant (resolves from /nix/store)
nix build .#lgx-portable   # Portable self-contained variant

# Run integration tests
nix build .#test-search-client
./result/bin/test_minimal
```

## LGX Outputs

| Output | Description | Use Case |
|--------|-------------|----------|
| `default` | Module library + headers | Development, dependency for other modules |
| `lgx` | Dev variant LGX bundle | Loading in Basecamp (dev mode) |
| `lgx-portable` | Portable LGX bundle | Distribution/deployment |
| `test-search-client` | Integration test binary | Manual testing against live IA API |

## API Reference

### Search

```
GET https://archive.org/advancedsearch.php?q=QUERY&rows=N&output=json
```

Returns JSON with `response.docs` array containing search results.

### Item Metadata

```
GET https://archive.org/metadata/<identifier>
```

Returns JSON with item metadata including title, creator, date, mediatype, etc.

## Testing

Run the integration test against live IA API:

```bash
./result/bin/test_minimal
```

This tests:
- Basic search functionality
- Mediatype filtering
- Item metadata retrieval
- Empty query handling

## Module Dependencies

- Qt 6 (Core, Network, RemoteObjects)
- logos-module (Logos platform SDK)
- logos-cpp-sdk (C++ SDK)

## License

MIT
