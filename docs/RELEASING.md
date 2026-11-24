# Releasing

Pushing a tag `vMAJOR.MINOR.PATCH[-suffix]` builds a signed release APK in GitHub Actions and publishes it as a
GitHub Release (`.github/workflows/android-release.yml`).

## One-time setup

The release is signed with your own keystore. Create it once and keep it safe: **if you lose it you can never update
installed apps**, and anyone who has it can sign as you. Never commit it (`*.jks` is git-ignored).

```bash
keytool -genkeypair -v -keystore release.jks -alias hiveshock -keyalg RSA -keysize 4096 -validity 10000
```

Then store the four values as repository secrets (Settings → Secrets and variables → Actions):

```bash
gh secret set ANDROID_KEYSTORE_BASE64   < <(base64 -w0 release.jks)
gh secret set ANDROID_KEYSTORE_PASSWORD
gh secret set ANDROID_KEY_ALIAS         # hiveshock
gh secret set ANDROID_KEY_PASSWORD
```

Back up `release.jks` and its passwords somewhere private outside the repository.

## Cutting a release

```bash
git switch main && git pull
git tag -a v9.2.3-hiveshock.1 -m "Short title" -m "What changed, one point per line"
git push origin main v9.2.3-hiveshock.1
```

- The tag message becomes the top of the release notes; GitHub appends the generated changelog (commits and PRs since
  the previous release). A lightweight tag (`git tag v…`) also works and gets only the generated changelog.
- Tags containing `alpha`, `beta`, `rc`, `pre` or `dev` (e.g. `v9.2.3-hiveshock.2-rc.1`) are published as
  pre-releases.
- `versionName` is the tag without the leading `v`. `versionCode` is `1000 + run number`, so it grows with every
  release and installs upgrade in place.
- Re-running the workflow for the same tag replaces the attached files instead of failing.

Each release has `Shipwright-HiveShock-<tag>.apk`, its `.sha256`, and a build-provenance attestation:

```bash
gh attestation verify Shipwright-HiveShock-<tag>.apk --owner <your-github-user>
```

## Download page (hive.yafel.dev)

When the **Release** workflow finishes successfully, `.github/workflows/notify-web.yml` sends a `repository_dispatch`
to `Lexharden/hive-web`, which rebuilds the site and shows the new APK under *Ocarina of Time → Android*. It needs one
more repository secret, `WEB_DISPATCH_TOKEN`: the same fine-grained token (Contents read and write on `hive-web`) used
by the other Hive repos. Without it only this notification fails; the release itself is unaffected, and the site still
picks the release up on its 6-hourly rebuild.

`notify-web.yml` listens for the workflow **by name**, so keep `name: Release` in `android-release.yml`.

## Test builds without a tag

Actions → **Android Test APK** → Run workflow. It builds a signed `dev.yafel.soh.test` APK (it can sit next to the
real app) and keeps it as a workflow artifact for 14 days.

## The libultraship patch

`libultraship` is a submodule pinned to a commit that lacks the Android data-root and touch-camera changes this port
uses (`Ship::Context::SetAndroidDataRootPath`, `MainActivity.setCameraTouchActive`). They are kept as
`patches/libultraship-android.patch` and applied by the workflows. If you change `libultraship` locally, regenerate it:

```bash
git -C libultraship diff --binary > patches/libultraship-android.patch
```

The cleaner long-term fix is to fork `libultraship`, commit those changes there and point `.gitmodules` at the fork;
then the patch step becomes a no-op and can be deleted.

## Local build

```bash
cd Android && ./gradlew installDebug      # debug build, installed on the connected device
```
