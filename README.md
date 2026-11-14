# Mod Status

[![Join Discord](https://img.shields.io/badge/Join-Discord-5865F2?style=for-the-badge&logo=discord&logoColor=white)](https://discord.gg/bX4RzunsxC)

Appends the loaded mod count to Stormworks' existing bottom-left version label:

`v1.15.23 64Bit (12 mods loaded)`

## As of SWML 4.6.1 this mod now comes with SWML core, and is thus obsolete

If SWML detects a failure, the suffix becomes `(12 mods loaded, fault found)`.
It remains until the game restarts; consult the SWML logs for details.

Requires SWML 4.3.0 or newer with native UI support. Install the `mod_status`
folder alongside the other mods and enable it in SWML. There are no settings.
It also works in single player.

The count includes operational DLL and data-only mods, including Mod Status
itself. Disabled mods and failed loads are excluded. A partially installed mod
can remain counted while also setting the fault flag. Detected load/install
failures, caught SDK callback faults, structured error logs and explicit mod
fault reports set the flag. It cannot predict crashes or detect every game bug.

The mod edits the game's existing version text through SWML's native UI API.
It preserves the original version and updates only when the count, fault flag
or native version changes. Unsupported UI layouts are left alone. No overlay,
render loop polling thread or extra simulation work is added.

This mod is made in compliance of Geometa EULA. Under Section 3 Paragraph 4, `You must not modify, merge, distribute, translate, reverse engineer, decompile, or try to obtain or use source code for the Geometa Services, except for mods as per section 5 or where permitted by applicable law.` In section 5, `You may create and share mods and other content for our games, provided they comply with this Agreement. You own the content you create, including the content created in game, mods created with Geometa tool kits, community websites, etc.`
