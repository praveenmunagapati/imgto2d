# Portable vpype runtime

The C++ app prefers the vpype console entry point inside a project-local Python runtime:

```text
runtime/python/Scripts/vpype.exe
```

Setup:

```powershell
.\scripts\setup_portable_vpype.ps1
```

Lookup order used by SVG export:

1. `runtime/python/Scripts/vpype.exe`
2. app-relative `runtime/python/Scripts/vpype.exe`
3. `.venv/Scripts/vpype.exe`
4. global `vpype` on `PATH`

`runtime/python/` is ignored by git because it is a generated dependency bundle.
