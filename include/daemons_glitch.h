#ifndef GUARD_DAEMONS_GLITCH_H
#define GUARD_DAEMONS_GLITCH_H

// T-486: THE ONE GLITCH ON PURPOSE (src/daemons_glitch.c). A few frames of scrambled scanlines on BG (0-3) and of
// the BG palettes in PALETTES (bit p: BG palette p) shuffled, then everything put back. The screen's VBlank must call
// ScanlineEffect_InitHBlankDmaTransfer (the field's and the battle's do; the book reader's does since T-486).
void Glitch_Start(u8 bg, u16 palettes);
bool8 Glitch_IsRunning(void);

#endif // GUARD_DAEMONS_GLITCH_H
