#ifndef GUARD_DAEMONS_GLITCH_H
#define GUARD_DAEMONS_GLITCH_H

// T-486: THE ONE GLITCH ON PURPOSE (src/daemons_glitch.c). A few frames of scrambled scanlines on BG (0-3) and of
// the BG palettes in PALETTES (bit p: BG palette p) shuffled, then everything put back. The screen's VBlank must call
// ScanlineEffect_InitHBlankDmaTransfer (the field's and the battle's do; the book reader's does since T-486).
// STRENGTH is 25-100 (T-486): 100 is 12 frames thrown +/-24px, the palettes rotated every other frame; 25 is 4 frames,
// +/-10px, rotated every 4th. Glitch_StrengthNow() is the clock's: 100 at 3:30, toward 25 at 3:00 and 3:59, 25 outside.
#define GLITCH_WEAKEST   25
#define GLITCH_STRONGEST 100
void Glitch_Start(u8 bg, u16 palettes, u8 strength);
u8 Glitch_StrengthNow(void);
bool8 Glitch_IsRunning(void);

#endif // GUARD_DAEMONS_GLITCH_H
