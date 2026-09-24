# Lugaru Community Preservation and Modding

## Goal

Lugaru-Web should preserve more than the executable and the three community campaigns already bundled with OSS Lugaru.

The historical ecosystem included:

- campaigns;
- standalone challenge maps;
- map packs;
- skins and clothing;
- texture replacements;
- custom characters;
- model/skeleton/animation work;
- weapon replacements/packs;
- music/audio mods;
- converters and Blender plugins;
- repair utilities;
- mod installers/downloaders;
- tutorials and forum knowledge.

The project should treat **finding an artifact** and **having permission to redistribute it** as separate questions.

---

# 1. Known historical campaigns

## Cleared and bundled by OSS

### Lugaru: Temple v1.0

- Author: Silb
- Historical release: 2006
- License: CC BY-SA 3.0
- OSS status: integrated
- Important precedent: OSS records say the final non-free files were replaced before completion.

### Lugaru: Empire v1.0

- Author: Jendraz
- Historical release: 2008
- License: CC BY-SA 4.0
- OSS status: integrated

### The Seven Tasks v1.0

- Author: Philtron R.
- Historical release: 2009
- License: CC BY-SA 4.0
- OSS status: integrated

## Surviving separately / unresolved or partially resolved

### Lugaru: Ancestral Tales 1.2

- Author: Kehaar
- OSS issue records a later CC BY-SA 4.0 relicensing of the author's contribution.
- OSS maintainers still raised concerns about some custom texture provenance.
- Candidate for a clean-room asset audit and restored distributable edition.

### Lugaru: Reluctant Assassin 1.0

- Author: Lotus Wolf
- Survives in the OSS `nonfree-mods` preservation repository.
- OSS attempted contact but did not document a completed public relicense.
- Do not bundle publicly until rights are resolved.

### Tannim's Mini Campaign

- Author: tannim
- Survives in the OSS `nonfree-mods` repository.
- OSS issue says it was waiting for a public license statement.
- Do not assume that free download implies redistribution permission.

---

# 2. Known standalone maps/map packs outside the bundled OSS campaigns

Historical Wolfire/wiki/forum evidence points to more content than the three bundled campaigns.

## eyeCube

- `eyeCube MapPack`
- Wolfire Wiki later noted a repaired link in March 2013.
- File/license status: investigate.

## Levbot

- `Map Pack 1`
- `Map Pack 2`
- also associated with other historical mods such as `Space Lugaru`.
- file/license/contact status: investigate.

## rudel_ic / Wolf Mathwig

Known nine-map pack:

- Arena
- House
- Hunt
- Trap
- Bigtower
- Tower
- Bridge
- Coward
- Stairs

Identity is unusually well established: Wolfire published a 2010 guest article identifying **Wolf Mathwig aka rudel_ic**, a German software developer and long-time forum member.

His importance is broader than maps: he reverse-engineered Lugaru binary formats and created tooling for 3D models, animations, and skeletons.

## BunnyWithStick

Known maps include:

- Forest Silly Forest
- Winter Wanderers
- Hill Grove
- Desert Wanderers
- Cave

File/license/contact status: investigate through forum identity/history.

## Philtron R. challenge maps

Philtron later wrote a detailed postmortem documenting **14 standalone challenge maps** and said they were downloadable from the Lugaru forum.

The OSS preservation issue also records that Philtron made maintainers aware of challenge maps relicensed under CC BY-SA 4.0. The recovered files should be matched carefully to the scope of that grant before bundling.

The postmortem itself is valuable design documentation because it explains how the maps worked, including arena waves created with long patrol routes in an engine with no general map scripting.

---

# 3. Historical mod categories beyond maps

The Wolfire Wiki's mod list documents a wider ecosystem including:

- Ninja Lugaru — Wouter
- Matrix — Zip
- China — Miro
- Space Lugaru — Levbot
- New School Samurai — skr
- Dumb T-shirts — Silb
- Various Hi-Res Textures — Maxis
- Long-haired Rabbit Skins — Jesse
- Alternate Leather Armor — Jesse
- Alternate Music — Ledood
- Custom Made Music — DJ Sick Nick
- Weapon Shop — Lotus Wolf

Some links were already dead by 2009. That makes web archaeology and filename recovery important.

---

# 4. Weapon Shop / Lotus Wolf

`The Weapon Shop` had a very large Wolfire forum thread and a separate historical site. Surviving forum pages discuss items such as a war hammer.

Research goals:

- reconstruct the weapon inventory;
- recover original archives/file names;
- determine whether items were model/texture replacements or involved later code modifications;
- identify original readmes and credits;
- determine license status;
- find a current public contact channel for Lotus Wolf.

Historical context matters because before the source release, Wolfire documentation said normal modding was largely limited to maps/textures. Custom weapons therefore often worked by replacing an existing model/asset slot rather than introducing an entirely new gameplay type.

For Lugaru-Web, a recovered historical weapon could eventually have two clearly different forms:

- **Historical** — behaves as it did in classic Lugaru.
- **Expanded** — optionally gains new mechanics while retaining the historical model/identity.

Do not silently rewrite the historical behavior.

---

# 5. Model, skeleton, and animation tooling

Wolf Mathwig / rudel_ic is central here.

Wolfire's 2010 guest post documents his process of reverse-engineering Lugaru's binary formats. His tool evolution included:

- a Java command-line converter (`X2Solid`);
- a later Java application (`Brainfart`);
- Python/Blender import/export tooling;
- support extending beyond static models toward skeletons and animations.

He specifically described the long-term goal of making Lugaru's models, skeletons, animations, and maps editable in Blender; by the time of the post he had achieved models/skeletons/animations but not maps.

This tooling is historically important even if it is obsolete today.

Preserve:

- original archives;
- source code;
- readmes;
- screenshots/examples;
- version/dependency notes;
- original URLs;
- forum threads;
- licenses;
- checksums.

A broken Blender 2.x/Python 2 plugin can still be a critical artifact because it reconstructs the missing community development toolchain.

---

# 6. Lugaru's lost/rediscovered toolchain

Historical documentation says Rosen's original private tools were not generally available to the modding community. Community members therefore reverse-engineered enough of the formats to build their own creation/conversion tools.

That means preservation should include the **knowledge** required to recreate content, not just the finished content.

Important categories:

- map editor documentation;
- dialogue format documentation;
- binary-format notes;
- Blender scripts;
- model converters;
- animation converters;
- skeleton tooling;
- MapFix or similar repair utilities;
- mod installers/downloaders.

---

# 7. Lugaru Downloader / historical mod database lead

Historical evidence points to an Adobe AIR-style downloader/manager used to install and revert Lugaru mods. Wolfire also discussed community map organization and an automatic mod downloader during early Overgrowth alpha development.

This may be one of the best routes to reconstructing a historical mod catalog.

Research targets:

- application binary/package;
- source code;
- hardcoded mod list;
- remote XML/JSON/config endpoints;
- cached database/catalog files;
- old screenshots/documentation;
- exact download URLs/filenames.

Even dead URLs are valuable because exact filenames can lead to mirrors or Wayback captures.

---

# 8. Archaeology sources

Systematically search:

- Wolfire Games forums;
- Wolfire Wiki;
- old Wolfire Lugaru pages;
- OSS preservation issues/merge requests;
- GitHub/GitLab forks;
- ModDB;
- Wayback Machine;
- personal sites/blogs of known modders;
- historical Linux/game-community mirrors;
- exact ZIP filenames;
- old videos/descriptions;
- old downloader catalogs;
- forum signatures/profile website links.

Avoid relying on a single surviving index. Triangulate authorship and dates from multiple sources where possible.

---

# 9. Contact leads

## rudel_ic

- Real identity publicly established as Wolf Mathwig.
- Historical forum profile listed Hamburg and a website field.
- High-priority contact/relicensing candidate.

## Lotus Wolf

- Reluctant Assassin author.
- Weapon Shop author/maintainer.
- Major forum presence and old site/video/contest breadcrumbs.
- OSS team reported difficulty establishing contact.

## tannim

- Historical campaign preserved.
- OSS issue suggests the problem was obtaining a public license statement rather than total loss of identity/contact.
- Historical Wolfire contest posts provide additional account/activity breadcrumbs.

## BunnyWithStick

- Forum identity and several known maps survive.
- Requires profile/signature/URL archaeology.

## Levbot / eyeCube

- Current public identity/contact remains weak.
- Work outward from original threads, filenames, profile IDs, archived pages, and linked sites rather than generic username searches.

## Rémi Verschelde

A useful meta-contact: he coordinated much of the 2016 OSS mod-preservation/relicensing work and may remember private/public leads not obvious from the surviving issue.

---

# 10. Licensing model

## Do not confuse free download with redistribution permission

An old mod being downloadable at no charge does not automatically grant the right to mirror, modify, or bundle it.

Likewise:

- dead site ≠ public domain;
- abandoned account ≠ public domain;
- open-source engine ≠ every mod is open source;
- CC-licensed stock assets ≠ the mapper's original arrangement is automatically CC-licensed.

## Map copyright in practical terms

A map author does not own Lugaru's engine, stock models, stock textures, or tools. Their potentially protectable contribution is the original material they created: layout, selection/arrangement, dialogue, terrain, enemy placement, custom assets, etc.

This is why a map can have its own rights status even when it is built entirely from stock open assets.

---

# 11. Relicensing workflow

When a creator is found, ask a precise question rather than "can we use your map?"

Recommended form:

> If you created these maps and control the rights to your original contributions, would you be willing to release your original contributions to [exact titles/files] under the Creative Commons Attribution-ShareAlike 4.0 International license (CC BY-SA 4.0), allowing them to be preserved, redistributed, and modified with attribution?

Also ask whether they created all custom textures/models/sounds included in the package and, if not, whether they remember the sources.

Preserve the evidence:

- original email/DM/forum URL;
- screenshot/PDF export as redundancy;
- date;
- exact works covered;
- requested attribution;
- license URL/name;
- original package checksum.

A public statement or license file is ideal, but a clear written grant from the rights holder is meaningful evidence.

---

# 12. Asset provenance audits

A CC license from a mod author only covers rights that author actually controls.

A historical ZIP may contain third-party material.

For every non-stock file:

1. hash it;
2. compare against known stock Lugaru releases;
3. compare against known community assets;
4. inspect filenames and metadata;
5. read README/forum credits;
6. search distinctive filenames;
7. reverse-image-search suspicious textures when useful;
8. trace historical websites/licenses through Wayback;
9. classify unresolved material explicitly.

Potential automated classification:

```text
Exact stock Lugaru asset       CLEAN
Modified stock Lugaru asset    REVIEW
Known community asset          CHECK LICENSE
Unknown custom asset           REVIEW
Map/campaign authored content  CHECK AUTHOR LICENSE
```

Do not claim certainty where provenance cannot be established.

---

# 13. Restored distributable editions

A strong historical map should not be discarded merely because one texture or music file has bad provenance.

Maintain two concepts:

## Original archival artifact

- untouched original package;
- original filenames;
- original questionable assets preserved for historical research where lawfully held;
- checksum and source/provenance record.

## Lugaru-Web restored edition

- only redistributable material;
- unknown/non-free assets removed or replaced;
- gameplay/map geometry preserved where possible;
- changes listed explicitly in a restoration note.

Example:

```text
RESTORATION.md

- `stone_wall.jpg` replaced: original provenance unknown.
- `battle.mp3` removed: third-party rights not established.
- replacement stone texture created for Lugaru-Web.
- no map geometry, enemy placement, or dialogue changed.
```

This directly follows the spirit of the OSS Temple cleanup, where remaining non-free files were replaced before integration.

---

# 14. Replacement assets

If the author contribution is licensed but an incidental asset is not, replacement is often straightforward.

Examples:

- unknown stone texture → new original/cleared stone texture;
- unknown wood texture → new original/cleared wood texture;
- commercial song → removed or replaced with cleared music;
- questionable model → new compatible model;
- unknown UI image → new graphic.

AI-generated replacement art may be one production option, but replacements should be new works serving the same generic visual function rather than attempts to reproduce a specific protected image exactly. Keep provenance/terms for the replacement just as carefully as historical provenance.

---

# 15. Archive policy

The archival copy and distributable copy should never be conflated.

Suggested metadata fields:

```text
Title
Author / handle
Real identity (only if publicly established and relevant)
Approximate date
Type
Original thread/page
Recovered from
Original filename
SHA-256
Known versions
Compatibility
Original license
Relicensed license
Relicensing evidence
Third-party assets
Rights status
Author contacted/date
Response
Public redistribution allowed?
Lugaru-Web restored edition?
Testing status
Notes
```

Potential rights statuses:

```text
GREEN   cleared for redistribution
YELLOW  survives, rights unresolved
RED     known artifact, surviving copy not yet recovered
WHITE   author/contact research in progress
```

---

# 16. User-imported content

Lugaru-Web may eventually support local import of historical/user-created map or campaign packages the user already has.

Conceptual UX:

```text
CUSTOM CONTENT
→ Import Lugaru Map/Campaign
→ choose local package
→ validate/identify format
→ load from browser-local storage
```

This is useful regardless of archival licensing because it supports people's own maps and historical packages without requiring every file to be bundled into the official distribution.

The project should still avoid presenting an unlicensed download catalog as if it were an official content repository.

---

# 17. Why preserve low-value mods too?

Not every artifact needs to be good enough to ship.

A goofy rabbit skin, obsolete converter, broken Python plugin, or one-joke map can still document:

- what the community was doing;
- what technical limitations they worked around;
- how tools evolved;
- how the game's aesthetics were remixed;
- which usernames/authors participated;
- how knowledge circulated through the forums.

Preservation is broader than curation.

---

# Primary references

- OSS mod relicensing/preservation issue: https://gitlab.com/osslugaru/lugaru/-/issues/31
- OSS content license manifest: https://github.com/osslugaru/lugaru/blob/master/CONTENT-LICENSE.txt
- Wolfire Lugaru Mods wiki: https://wiki.wolfire.com/Lugaru_Mods
- Wolfire Lugaru FAQ: https://wiki.wolfire.com/Lugaru_FAQ
- Wolfire map-editing documentation: https://wiki.wolfire.com/Map_Editing_in_Lugaru
- Wolf Mathwig / rudel_ic reverse-engineering article: https://www.wolfire.com/blog/2010/04/Reverse-Engineering-Binary-Files
- Philtron R., 14 Challenge Maps postmortem: https://electriccartilage.wordpress.com/2017/06/30/postmortem-14-lugaru-challenge-maps-i-made/
- Surviving Weapon Shop forum page: https://forums.wolfire.com/viewtopic.php?p=97502

---

# 18. Compatibility archaeology is part of preservation

The 2026-09-23 source/data pass reinforces that preserving Lugaru content means preserving undocumented **authoring behavior**, not only retaining map files and assets.

Historical maps can depend on implementation details that look like malformed or obsolete data in a modern schema.

Verified examples include:

- dialogue type integers acting as small scripting opcodes rather than simple categories;
- dialogue participants stored as direct Person indices and teleported to authored scene positions with no automatic restoration;
- historical `#` padding inside dialogue strings, still stripped by the renderer, originating in an old workaround for exact 10/13-character dialogue crashes;
- `Hotspot` values used for contextual text, Person-following text, target kills, exit/win volumes, and dialogue launchers depending on numeric range;
- inert size-zero `type = -111`, text `mapname` hotspot records surviving in converted JSON as remnants of the old binary demo-marker path;
- `ChooseNext = 2` performing an unusual stealth/in-place scene swap that preserves existing physical objects and Turner position while replacing much of the authored gameplay state;
- campaign/world-map coordinates stored as fixed positions over `World.png` rather than as a generalized graph/layout system;
- map `maptype` values that do not by themselves express every actual win rule because hotspots/dialogues also participate in completion;
- patrol waypoints and global pathfinding nodes being separate authoring systems;
- loose weapons existing physically at runtime while historical save/load persists weapons through Person ownership, causing ground weapons to disappear after a map save/reload;
- `mapcenter/mapradius` representing editor/map-boundary metadata while runtime world containment uses a different terrain/viewdistance-derived clamp.

These details should be captured in preserved readmes/tool documentation and in any future import validator.

## Preservation rule for suspicious data

When importing an old map, do not silently normalize a strange value just because the modern engine appears not to need it.

Prefer:

```text
preserve original bytes/package when possible
→ identify/convert into a lossless compatibility representation
→ warn about suspicious or unsupported fields
→ test behavior against Classic
→ only produce a cleaned/restored derivative as a separately documented artifact
```

This is particularly important for numeric dialogue/hotspot fields because an apparently arbitrary integer may be executable level logic.

## Historical authoring references worth archiving locally

In addition to finished mods, preserve snapshots of surviving Wolfire documentation for:

- map editing and campaign `main.txt` format;
- world-map coordinate authoring;
- dialogue authoring/participants;
- hotspot/objective conventions;
- patrol vs. pathfinding nodes;
- console map/environment commands;
- the historical dialogue-string crash workaround;
- weapon placement/save limitations;
- challenge scoring/award descriptions.

The Wiki can explain why a strange field exists even when the modern code no longer exposes an editor control for it.

## Importer/restoration metadata implication

A future restoration manifest should be able to distinguish:

```text
original artifact
lossless format conversion
compatibility repair
asset-rights replacement
behavioral modernization
```

A compatibility repair such as restoring a broken hotspot reference is historically different from redesigning the map around a new objective system.
