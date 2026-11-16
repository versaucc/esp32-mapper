# Contributing

## Workflow

1. **One branch per task.** Branch from an up-to-date `main`:
   ```bash
   git switch main && git pull
   git switch -c task/<subsystem>-<short-description>   # e.g. task/sonar-timeout-handling
   ```
2. **Small commits** with a clear message in the imperative mood
   (`Add sonar timeout handling`, not `fixed stuff`).
3. **Check the build** before you push: `pio run` must pass locally.
   CI runs the same build on every push.
4. **Open a pull request** into `main` using the PR template.
5. **A teammate reviews.** Nobody merges their own PR. The reviewer checks the
   code, the bench test notes, and that `config.h` holds no real credentials.
6. **Squash or merge** once approved and CI is green. Delete the branch afterwards.

Do not push directly to `main`.

## Subsystems and TODOs

Each task belongs to one subsystem. Name it in the branch, PR title, and task issue:

| Subsystem | Covers |
| --- | --- |
| `sonar` | `firmware/src/sonar.*` |
| `servo` | `firmware/src/servo_sweep.*` |
| `net` | `firmware/src/net.*`, Wi-Fi and WebSocket |
| `web` | `firmware/data/*`, `web-prototype/` |
| `hardware` | `hardware/kicad/`, wiring |
| `mechanical` | `mechanical/` |
| `docs` | `docs/` |
| `tests` | `tests/` bench tests and logs |

Mark unfinished code with a comment naming the owner subsystem, for example
`// TODO(servo): ...`. Search with `grep -rn "TODO(" firmware/`.

## Style

- Pins, timing, and credentials go in `firmware/include/config.h`, not in `.cpp` files.
- Keep `loop()` non-blocking where practical.
- Use 2-space indent in web files, 2-space indent in C++ as in the existing code.

## Logging AI use

If you use an AI tool for a task, add a line to [docs/ai-use-log.md](docs/ai-use-log.md)
in the same PR.
