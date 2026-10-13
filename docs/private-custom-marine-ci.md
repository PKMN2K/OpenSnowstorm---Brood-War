# Private GitHub Actions custom-unit test

This manually launched workflow tests unit ID 228 without checking proprietary assets into the public engine repository.

## One-time setup

1. Create a **private GitHub repository** that you control, for example `PKMN2K/broodwar-test-assets`.
2. In that private repository, create a release (for example tag `marine-test-v1`) and attach these four files with the **exact filenames**:
   - `StarDat.mpq`
   - `BrooDat.mpq`
   - `Patch_rt.mpq`
   - `custom_marine.scx`
3. Use an MPQ-compatible editor to make `custom_marine.scx` a UMS map containing an ID 228 Marine owned by an active player. See [custom-marine-smoke-test.md](custom-marine-smoke-test.md) for the extracted-CHK patching tool.
4. Generate a fine-grained GitHub personal access token with **Contents: Read-only** permission for the private assets repository. Don't include the token in messages, code, or the public repository.
5. In the public `PKMN2K/OpenSnowstorm---Brood-War` repository, add an Actions secret named `OPENSNOWSTORM_ASSETS_TOKEN` containing that token.

## Run the test

Open **Actions → Private Brood War custom-unit map test → Run workflow**. Select `feature/expanded-unit-system`, then enter:

- **assets_repository:** `PKMN2K/broodwar-test-assets` (or the name you created)
- **release_tag:** `marine-test-v1` (or your release tag)

The workflow downloads the release privately to the ephemeral runner, builds `gfxtest`, and executes `scripts/test_custom_marine.sh`. It checks that ID 228 is registered, that a unit instance with an associated sprite exists, that the duplicate costs 125 minerals, and that the vanilla Marine remains 50 minerals.

**Important limitations:** This test does not yet check rendered image pixels, selection, movement, shooting, or damage. It does not upload private data as an artifact or cache. Logs may still contain engine diagnostic messages; keep diagnostic output free of proprietary content or credentials.

**Security:** Files from releases in the private assets repo remain accessible to people and workflows authorized to read that repo. They are not committed to the public OpenSnowstorm repo. Deleting the runner's copy is a cleanup step, not a guarantee that no data can ever appear elsewhere. Do not publish copyrighted files or make the private assets repository public.
