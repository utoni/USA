# USA

## The "Untitled Sidescroll Adventure" ;)

Nothing special, just tryin' OpenGL.
Get in touch if you can draw Pixel Art Graphics / Sprites.

Graphics looks like it was painted by a 3-year old.

## Moon godrays (volumetric light shafts)

The fullscreen framebuffer pass now includes a pixel-art friendly moon godrays effect.

- `G` toggles godrays on/off.
- `M` switches source mode:
  - `Moon sprite source` (uses configured screen-space light source positions)
  - `Directional off-screen source` (uses a normalized moonlight direction)
- `B` toggles debug mask visualization (shows the sampled occlusion mask).
- `F` toggles firefly particles on/off.
- `-` / `=` lowers/raises firefly intensity.

In moon sprite mode, multiple emitters can be configured (moon + stars) via
`Framebuffer::AddGodraysLightSource(...)`.
For performance, sprite mode processes up to 4 emitters per pixel and shares the sample budget across them.

### Tuning

Godrays parameters live in `Framebuffer.hpp` inside `GodraysParameters`:

- `Intensity`: overall contribution of rays
- `Exposure`: final post-exposure multiplier
- `Decay`: attenuation per sample along ray march
- `Density`: how far each sample step travels toward source
- `Weight`: per-sample accumulation weight
- `Samples`: number of radial samples (lower = cheaper/sharper for pixel art)
- `Color`: moonlight tint
- `NoiseAmount`: subtle temporal dithering/noise to avoid over-smooth blur

## Night scene layer additions

The parallax stack now includes extra dark/night pixel layers:

- `sky_stars.png` (distant stars + silhouette band)
- `clouds_far.png` (dim high-altitude cloud layer)
- `fog_far.png` (slow drifting low fog layer)

All are loaded from `assets/` with nearest-neighbor filtering via existing texture setup.

### Firefly shader particles

`shaders/framebuffer.fragment.glsl` now includes a lightweight fullscreen firefly pass.

Tune defaults in `Framebuffer.hpp` (`FireflyParameters`):

- `Intensity`
- `Density`
- `Size`
- `Speed`
