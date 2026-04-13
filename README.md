# USA

## The "Untitled Sidescroll Adventure" ;)

Nothing special, just tryin' OpenGL.
Get in touch if you can draw Pixel Art Graphics / Sprites.

Graphics looks like it was painted by a 3-year old.

## Moon godrays (volumetric light shafts)

The fullscreen framebuffer pass now includes a pixel-art friendly moon godrays effect.

- `G` toggles godrays on/off.
- `M` switches source mode:
  - `Moon sprite source` (uses the moon screen position)
  - `Directional off-screen source` (uses a normalized moonlight direction)
- `B` toggles debug mask visualization (shows the sampled occlusion mask).

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
