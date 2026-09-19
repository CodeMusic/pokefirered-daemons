#include "constants/battle.h"
#include "constants/battle_anim.h"
#include "constants/battle_string_ids.h"
#include "constants/rgb.h"
#include "constants/songs.h"
#include "constants/sound.h"
#include "constants/moves.h"
	.include "asm/macros.inc"
	.include "asm/macros/battle_anim_script.inc"
	.include "constants/constants.inc"

	.section script_data, "aw", %progbits

	.align 2
gMovesWithQuietBGM::
	.2byte MOVE_SING
	.2byte MOVE_PERISH_SONG
	.2byte MOVE_GRASS_WHISTLE
	.2byte 0xFFFF

	.align 2
gBattleAnims_Moves::
	.4byte Move_NONE
	.4byte Move_POUND
	.4byte Move_KARATE_CHOP
	.4byte Move_DOUBLE_SLAP
	.4byte Move_COMET_PUNCH
	.4byte Move_MEGA_PUNCH
	.4byte Move_PAY_DAY
	.4byte Move_FIRE_PUNCH
	.4byte Move_ICE_PUNCH
	.4byte Move_THUNDER_PUNCH
	.4byte Move_SCRATCH
	.4byte Move_VICE_GRIP
	.4byte Move_GUILLOTINE
	.4byte Move_RAZOR_WIND
	.4byte Move_SWORDS_DANCE
	.4byte Move_CUT
	.4byte Move_GUST
	.4byte Move_WING_ATTACK
	.4byte Move_WHIRLWIND
	.4byte Move_FLY
	.4byte Move_BIND
	.4byte Move_SLAM
	.4byte Move_VINE_WHIP
	.4byte Move_STOMP
	.4byte Move_DOUBLE_KICK
	.4byte Move_MEGA_KICK
	.4byte Move_JUMP_KICK
	.4byte Move_ROLLING_KICK
	.4byte Move_SAND_ATTACK
	.4byte Move_HEADBUTT
	.4byte Move_HORN_ATTACK
	.4byte Move_FURY_ATTACK
	.4byte Move_HORN_DRILL
	.4byte Move_TACKLE
	.4byte Move_BODY_SLAM
	.4byte Move_WRAP
	.4byte Move_TAKE_DOWN
	.4byte Move_THRASH
	.4byte Move_DOUBLE_EDGE
	.4byte Move_TAIL_WHIP
	.4byte Move_POISON_STING
	.4byte Move_TWINEEDLE
	.4byte Move_PIN_MISSILE
	.4byte Move_LEER
	.4byte Move_BITE
	.4byte Move_GROWL
	.4byte Move_ROAR
	.4byte Move_SING
	.4byte Move_SUPERSONIC
	.4byte Move_SONIC_BOOM
	.4byte Move_DISABLE
	.4byte Move_ACID
	.4byte Move_EMBER
	.4byte Move_FLAMETHROWER
	.4byte Move_MIST
	.4byte Move_WATER_GUN
	.4byte Move_HYDRO_PUMP
	.4byte Move_SURF
	.4byte Move_ICE_BEAM
	.4byte Move_BLIZZARD
	.4byte Move_PSYBEAM
	.4byte Move_BUBBLE_BEAM
	.4byte Move_AURORA_BEAM
	.4byte Move_HYPER_BEAM
	.4byte Move_PECK
	.4byte Move_DRILL_PECK
	.4byte Move_SUBMISSION
	.4byte Move_LOW_KICK
	.4byte Move_COUNTER
	.4byte Move_SEISMIC_TOSS
	.4byte Move_STRENGTH
	.4byte Move_ABSORB
	.4byte Move_MEGA_DRAIN
	.4byte Move_LEECH_SEED
	.4byte Move_GROWTH
	.4byte Move_RAZOR_LEAF
	.4byte Move_SOLAR_BEAM
	.4byte Move_POISON_POWDER
	.4byte Move_STUN_SPORE
	.4byte Move_SLEEP_POWDER
	.4byte Move_PETAL_DANCE
	.4byte Move_STRING_SHOT
	.4byte Move_DRAGON_RAGE
	.4byte Move_FIRE_SPIN
	.4byte Move_THUNDER_SHOCK
	.4byte Move_THUNDERBOLT
	.4byte Move_THUNDER_WAVE
	.4byte Move_THUNDER
	.4byte Move_ROCK_THROW
	.4byte Move_EARTHQUAKE
	.4byte Move_FISSURE
	.4byte Move_DIG
	.4byte Move_TOXIC
	.4byte Move_CONFUSION
	.4byte Move_PSYCHIC
	.4byte Move_HYPNOSIS
	.4byte Move_MEDITATE
	.4byte Move_AGILITY
	.4byte Move_QUICK_ATTACK
	.4byte Move_RAGE
	.4byte Move_TELEPORT
	.4byte Move_NIGHT_SHADE
	.4byte Move_MIMIC
	.4byte Move_SCREECH
	.4byte Move_DOUBLE_TEAM
	.4byte Move_RECOVER
	.4byte Move_HARDEN
	.4byte Move_MINIMIZE
	.4byte Move_SMOKESCREEN
	.4byte Move_CONFUSE_RAY
	.4byte Move_WITHDRAW
	.4byte Move_DEFENSE_CURL
	.4byte Move_BARRIER
	.4byte Move_LIGHT_SCREEN
	.4byte Move_HAZE
	.4byte Move_REFLECT
	.4byte Move_FOCUS_ENERGY
	.4byte Move_BIDE
	.4byte Move_METRONOME
	.4byte Move_MIRROR_MOVE
	.4byte Move_SELF_DESTRUCT
	.4byte Move_EGG_BOMB
	.4byte Move_LICK
	.4byte Move_SMOG
	.4byte Move_SLUDGE
	.4byte Move_BONE_CLUB
	.4byte Move_FIRE_BLAST
	.4byte Move_WATERFALL
	.4byte Move_CLAMP
	.4byte Move_SWIFT
	.4byte Move_SKULL_BASH
	.4byte Move_SPIKE_CANNON
	.4byte Move_CONSTRICT
	.4byte Move_AMNESIA
	.4byte Move_KINESIS
	.4byte Move_SOFT_BOILED
	.4byte Move_HI_JUMP_KICK
	.4byte Move_GLARE
	.4byte Move_DREAM_EATER
	.4byte Move_POISON_GAS
	.4byte Move_BARRAGE
	.4byte Move_LEECH_LIFE
	.4byte Move_LOVELY_KISS
	.4byte Move_SKY_ATTACK
	.4byte Move_TRANSFORM
	.4byte Move_BUBBLE
	.4byte Move_DIZZY_PUNCH
	.4byte Move_SPORE
	.4byte Move_FLASH
	.4byte Move_PSYWAVE
	.4byte Move_SPLASH
	.4byte Move_ACID_ARMOR
	.4byte Move_CRABHAMMER
	.4byte Move_EXPLOSION
	.4byte Move_FURY_SWIPES
	.4byte Move_BONEMERANG
	.4byte Move_REST
	.4byte Move_ROCK_SLIDE
	.4byte Move_HYPER_FANG
	.4byte Move_SHARPEN
	.4byte Move_CONVERSION
	.4byte Move_TRI_ATTACK
	.4byte Move_SUPER_FANG
	.4byte Move_SLASH
	.4byte Move_SUBSTITUTE
	.4byte Move_STRUGGLE
	.4byte Move_SKETCH
	.4byte Move_TRIPLE_KICK
	.4byte Move_THIEF
	.4byte Move_SPIDER_WEB
	.4byte Move_MIND_READER
	.4byte Move_NIGHTMARE
	.4byte Move_FLAME_WHEEL
	.4byte Move_SNORE
	.4byte Move_CURSE
	.4byte Move_FLAIL
	.4byte Move_CONVERSION_2
	.4byte Move_AEROBLAST
	.4byte Move_COTTON_SPORE
	.4byte Move_REVERSAL
	.4byte Move_SPITE
	.4byte Move_POWDER_SNOW
	.4byte Move_PROTECT
	.4byte Move_MACH_PUNCH
	.4byte Move_SCARY_FACE
	.4byte Move_FAINT_ATTACK
	.4byte Move_SWEET_KISS
	.4byte Move_BELLY_DRUM
	.4byte Move_SLUDGE_BOMB
	.4byte Move_MUD_SLAP
	.4byte Move_OCTAZOOKA
	.4byte Move_SPIKES
	.4byte Move_ZAP_CANNON
	.4byte Move_FORESIGHT
	.4byte Move_DESTINY_BOND
	.4byte Move_PERISH_SONG
	.4byte Move_ICY_WIND
	.4byte Move_DETECT
	.4byte Move_BONE_RUSH
	.4byte Move_LOCK_ON
	.4byte Move_OUTRAGE
	.4byte Move_SANDSTORM
	.4byte Move_GIGA_DRAIN
	.4byte Move_ENDURE
	.4byte Move_CHARM
	.4byte Move_ROLLOUT
	.4byte Move_FALSE_SWIPE
	.4byte Move_SWAGGER
	.4byte Move_MILK_DRINK
	.4byte Move_SPARK
	.4byte Move_FURY_CUTTER
	.4byte Move_STEEL_WING
	.4byte Move_MEAN_LOOK
	.4byte Move_ATTRACT
	.4byte Move_SLEEP_TALK
	.4byte Move_HEAL_BELL
	.4byte Move_RETURN
	.4byte Move_PRESENT
	.4byte Move_FRUSTRATION
	.4byte Move_SAFEGUARD
	.4byte Move_PAIN_SPLIT
	.4byte Move_SACRED_FIRE
	.4byte Move_MAGNITUDE
	.4byte Move_DYNAMIC_PUNCH
	.4byte Move_MEGAHORN
	.4byte Move_DRAGON_BREATH
	.4byte Move_BATON_PASS
	.4byte Move_ENCORE
	.4byte Move_PURSUIT
	.4byte Move_RAPID_SPIN
	.4byte Move_SWEET_SCENT
	.4byte Move_IRON_TAIL
	.4byte Move_METAL_CLAW
	.4byte Move_VITAL_THROW
	.4byte Move_MORNING_SUN
	.4byte Move_SYNTHESIS
	.4byte Move_MOONLIGHT
	.4byte Move_HIDDEN_POWER
	.4byte Move_CROSS_CHOP
	.4byte Move_TWISTER
	.4byte Move_RAIN_DANCE
	.4byte Move_SUNNY_DAY
	.4byte Move_CRUNCH
	.4byte Move_MIRROR_COAT
	.4byte Move_PSYCH_UP
	.4byte Move_EXTREME_SPEED
	.4byte Move_ANCIENT_POWER
	.4byte Move_SHADOW_BALL
	.4byte Move_FUTURE_SIGHT
	.4byte Move_ROCK_SMASH
	.4byte Move_WHIRLPOOL
	.4byte Move_BEAT_UP
	.4byte Move_FAKE_OUT
	.4byte Move_UPROAR
	.4byte Move_STOCKPILE
	.4byte Move_SPIT_UP
	.4byte Move_SWALLOW
	.4byte Move_HEAT_WAVE
	.4byte Move_HAIL
	.4byte Move_TORMENT
	.4byte Move_FLATTER
	.4byte Move_WILL_O_WISP
	.4byte Move_MEMENTO
	.4byte Move_FACADE
	.4byte Move_FOCUS_PUNCH
	.4byte Move_SMELLING_SALT
	.4byte Move_FOLLOW_ME
	.4byte Move_NATURE_POWER
	.4byte Move_CHARGE
	.4byte Move_TAUNT
	.4byte Move_HELPING_HAND
	.4byte Move_TRICK
	.4byte Move_ROLE_PLAY
	.4byte Move_WISH
	.4byte Move_ASSIST
	.4byte Move_INGRAIN
	.4byte Move_SUPERPOWER
	.4byte Move_MAGIC_COAT
	.4byte Move_RECYCLE
	.4byte Move_REVENGE
	.4byte Move_BRICK_BREAK
	.4byte Move_YAWN
	.4byte Move_KNOCK_OFF
	.4byte Move_ENDEAVOR
	.4byte Move_ERUPTION
	.4byte Move_SKILL_SWAP
	.4byte Move_IMPRISON
	.4byte Move_REFRESH
	.4byte Move_GRUDGE
	.4byte Move_SNATCH
	.4byte Move_SECRET_POWER
	.4byte Move_DIVE
	.4byte Move_ARM_THRUST
	.4byte Move_CAMOUFLAGE
	.4byte Move_TAIL_GLOW
	.4byte Move_LUSTER_PURGE
	.4byte Move_MIST_BALL
	.4byte Move_FEATHER_DANCE
	.4byte Move_TEETER_DANCE
	.4byte Move_BLAZE_KICK
	.4byte Move_MUD_SPORT
	.4byte Move_ICE_BALL
	.4byte Move_NEEDLE_ARM
	.4byte Move_SLACK_OFF
	.4byte Move_HYPER_VOICE
	.4byte Move_POISON_FANG
	.4byte Move_CRUSH_CLAW
	.4byte Move_BLAST_BURN
	.4byte Move_HYDRO_CANNON
	.4byte Move_METEOR_MASH
	.4byte Move_ASTONISH
	.4byte Move_WEATHER_BALL
	.4byte Move_AROMATHERAPY
	.4byte Move_FAKE_TEARS
	.4byte Move_AIR_CUTTER
	.4byte Move_OVERHEAT
	.4byte Move_ODOR_SLEUTH
	.4byte Move_ROCK_TOMB
	.4byte Move_SILVER_WIND
	.4byte Move_METAL_SOUND
	.4byte Move_GRASS_WHISTLE
	.4byte Move_TICKLE
	.4byte Move_COSMIC_POWER
	.4byte Move_WATER_SPOUT
	.4byte Move_SIGNAL_BEAM
	.4byte Move_SHADOW_PUNCH
	.4byte Move_EXTRASENSORY
	.4byte Move_SKY_UPPERCUT
	.4byte Move_SAND_TOMB
	.4byte Move_SHEER_COLD
	.4byte Move_MUDDY_WATER
	.4byte Move_BULLET_SEED
	.4byte Move_AERIAL_ACE
	.4byte Move_ICICLE_SPEAR
	.4byte Move_IRON_DEFENSE
	.4byte Move_BLOCK
	.4byte Move_HOWL
	.4byte Move_DRAGON_CLAW
	.4byte Move_FRENZY_PLANT
	.4byte Move_BULK_UP
	.4byte Move_BOUNCE
	.4byte Move_MUD_SHOT
	.4byte Move_POISON_TAIL
	.4byte Move_COVET
	.4byte Move_VOLT_TACKLE
	.4byte Move_MAGICAL_LEAF
	.4byte Move_WATER_SPORT
	.4byte Move_CALM_MIND
	.4byte Move_LEAF_BLADE
	.4byte Move_DRAGON_DANCE
	.4byte Move_ROCK_BLAST
	.4byte Move_SHOCK_WAVE
	.4byte Move_WATER_PULSE
	.4byte Move_DOOM_DESIRE
	.4byte Move_PSYCHO_BOOST
	.4byte Move_PIN_MISSILE      @ CONSENSUS: converging projectiles
	.4byte Move_RECURSION        @ RECURSION: its own script since T-161 (it played PSYCHO BOOST's)
	.4byte Move_COUNT @ cannot be reached, because last move is Psycho Boost

	.align 2
gBattleAnims_StatusConditions::
	.4byte Status_Poison                    @ B_ANIM_STATUS_PSN
	.4byte Status_Confusion                 @ B_ANIM_STATUS_CONFUSION
	.4byte Status_Burn                      @ B_ANIM_STATUS_BRN
	.4byte Status_Infatuation               @ B_ANIM_STATUS_INFATUATION
	.4byte Status_Sleep                     @ B_ANIM_STATUS_SLP
	.4byte Status_Paralysis                 @ B_ANIM_STATUS_PRZ
	.4byte Status_Freeze                    @ B_ANIM_STATUS_FRZ
	.4byte Status_Curse                     @ B_ANIM_STATUS_CURSED
	.4byte Status_Nightmare                 @ B_ANIM_STATUS_NIGHTMARE

	.align 2
gBattleAnims_General::
	.4byte General_CastformChange           @ B_ANIM_CASTFORM_CHANGE
	.4byte General_StatsChange              @ B_ANIM_STATS_CHANGE
	.4byte General_SubstituteFade           @ B_ANIM_SUBSTITUTE_FADE
	.4byte General_SubstituteAppear         @ B_ANIM_SUBSTITUTE_APPEAR
	.4byte General_BaitThrow                @ B_ANIM_BAIT_THROW
	.4byte General_ItemKnockoff             @ B_ANIM_ITEM_KNOCKOFF
	.4byte General_TurnTrap                 @ B_ANIM_TURN_TRAP
	.4byte General_HeldItemEffect           @ B_ANIM_HELD_ITEM_EFFECT
	.4byte General_SmokeballEscape          @ B_ANIM_SMOKEBALL_ESCAPE
	.4byte General_FocusBand                @ B_ANIM_FOCUS_BAND
	.4byte General_Rain                     @ B_ANIM_RAIN_CONTINUES
	.4byte General_Sun                      @ B_ANIM_SUN_CONTINUES
	.4byte General_Sandstorm                @ B_ANIM_SANDSTORM_CONTINUES
	.4byte General_Hail                     @ B_ANIM_HAIL_CONTINUES
	.4byte General_LeechSeedDrain           @ B_ANIM_LEECH_SEED_DRAIN
	.4byte General_MonHit                   @ B_ANIM_MON_HIT
	.4byte General_ItemSteal                @ B_ANIM_ITEM_STEAL
	.4byte General_SnatchMove               @ B_ANIM_SNATCH_MOVE
	.4byte General_FutureSightHit           @ B_ANIM_FUTURE_SIGHT_HIT
	.4byte General_DoomDesireHit            @ B_ANIM_DOOM_DESIRE_HIT
	.4byte General_FocusPunchSetUp          @ B_ANIM_FOCUS_PUNCH_SETUP
	.4byte General_IngrainHeal              @ B_ANIM_INGRAIN_HEAL
	.4byte General_WishHeal                 @ B_ANIM_WISH_HEAL
	.4byte General_MonScared                @ B_ANIM_MON_SCARED
	.4byte General_GhostGetOut              @ B_ANIM_GHOST_GET_OUT
	.4byte General_SilphScoped              @ B_ANIM_SILPH_SCOPED
	.4byte General_SafariRockThrow          @ B_ANIM_ROCK_THROW
	.4byte General_SafariReaction           @ B_ANIM_SAFARI_REACTION

	.align 2
gBattleAnims_Special::
	.4byte Special_LevelUp                  @ B_ANIM_LVL_UP
	.4byte Special_SwitchOutPlayerMon       @ B_ANIM_SWITCH_OUT_PLAYER_MON
	.4byte Special_SwitchOutOpponentMon     @ B_ANIM_SWITCH_OUT_OPPONENT_MON
	.4byte Special_BallThrow                @ B_ANIM_BALL_THROW
	.4byte Special_BallThrowWithTrainer     @ B_ANIM_BALL_THROW_WITH_TRAINER
	.4byte Special_SubstituteToMon          @ B_ANIM_SUBSTITUTE_TO_MON
	.4byte Special_MonToSubstitute          @ B_ANIM_MON_TO_SUBSTITUTE

Move_NONE:
Move_MIRROR_MOVE:
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_POUND:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_DOUBLE_SLAP, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 4, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_DOUBLE_SLAP:
	playsewithpan SE_M_DOUBLE_SLAP, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: AFFLICT (T-134, vision.md 9.24) approved.
Move_POISON_POWDER:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(10, 11, 6)
	waitforvisualfinish
	playsewithpan SE_M_POISON_POWDER, SOUND_PAN_TARGET
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 0, 4, RGB(10, 11, 6)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 4, 8, RGB(10, 11, 6)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 8, 12, RGB(10, 11, 6)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 12, 0, RGB(10, 11, 6)
	waitforvisualfinish
	end
@ genanims: AFFLICT (T-134, vision.md 9.24) approved.
Move_STUN_SPORE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(11, 19, 12)
	waitforvisualfinish
	playsewithpan SE_M_POISON_POWDER, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 8, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(11, 19, 12)
	waitforvisualfinish
	end
@ genanims: AFFLICT (T-134, vision.md 9.24) approved.
Move_SLEEP_POWDER:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(11, 19, 12)
	waitforvisualfinish
	playsewithpan SE_M_POISON_POWDER, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 6, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 0, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 3, 0, 12, RGB(13, 13, 13)
	waitforvisualfinish
	delay 16
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 12, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_SWIFT:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_SWIFT, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_STRENGTH:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_TAKE_DOWN, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 5, 0, 7, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 14, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 14, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_TACKLE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_COMET_PUNCH, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 4, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_BODY_SLAM:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_TAKE_DOWN, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: AFFLICT (T-134, vision.md 9.24) approved.
Move_SUPERSONIC:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_SUPERSONIC, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 6, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: LOWER (T-134, vision.md 9.24) approved.
Move_SCREECH:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_SCREECH, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 5, RGB(13, 13, 13)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 5, 10, RGB(13, 13, 13)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 10, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: ENTROPY (T-134, vision.md 9.24) approved.
Move_FLAME_WHEEL:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 7, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 7, 0, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 7, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 7, 0, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 7, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 7, 0, RGB(27, 19, 5)
	waitforvisualfinish
	playsewithpan SE_M_FLAME_WHEEL, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 8, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 3, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 8, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 1, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 1, 6, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 6, 0, RGB(27, 19, 5)
	waitforvisualfinish
	end
@ genanims: SWARM (T-134, vision.md 9.24) approved.
Move_PIN_MISSILE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(16, 18, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_JUMP_KICK, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 4, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_JUMP_KICK, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 8, RGB(16, 18, 9)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(16, 18, 9)
	waitforvisualfinish
	end
@ genanims: FROZEN (T-134, vision.md 9.24) approved.
Move_ICICLE_SPEAR:
	playsewithpan SE_M_ICY_WIND, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(20, 25, 27)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(20, 25, 27)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_TAKE_DOWN:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_TAKE_DOWN, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 6, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 13, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 13, 0, RGB(24, 23, 21)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_DOUBLE_EDGE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_SWIFT, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 5, 0, 7, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 14, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 14, 0, RGB(24, 23, 21)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: CORRUPT (T-134, vision.md 9.24) approved.
Move_POISON_STING:
	playsewithpan SE_M_RAZOR_WIND2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 8, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 2, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 2, 6, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 1, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 1, 4, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 1, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 1, 0, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 3, 0, 3, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 3, 3, 0, RGB(10, 11, 6)
	waitforvisualfinish
	end
@ genanims: SWARM (T-134, vision.md 9.24) approved.
Move_TWINEEDLE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(16, 18, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_RAZOR_WIND2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 4, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_RAZOR_WIND2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 8, RGB(16, 18, 9)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(16, 18, 9)
	waitforvisualfinish
	end
@ genanims: ENTROPY (T-134, vision.md 9.24) approved.
Move_FIRE_BLAST:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(27, 19, 5)
	waitforvisualfinish
	playsewithpan SE_M_FLAME_WHEEL, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 10, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 14, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 14, 4, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 11, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 2, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 2, 8, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 4, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 12, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 2, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 2, 7, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 7, 0, RGB(27, 19, 5)
	waitforvisualfinish
	playsewithpan SE_M_FLAME_WHEEL, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_BATTLERS, 0, 0, 12, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_BATTLERS, 2, 12, 0, RGB(27, 19, 5)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_LEECH_SEED:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(11, 19, 12)
	waitforvisualfinish
	playsewithpan SE_M_POISON_POWDER, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 7, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 7, 0, RGB(11, 19, 12)
	waitforvisualfinish
	delay 4
	playsewithpan SE_M_POISON_POWDER, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 7, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 7, 0, RGB(11, 19, 12)
	waitforvisualfinish
	delay 4
	playsewithpan SE_M_POISON_POWDER, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 7, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 7, 0, RGB(11, 19, 12)
	waitforvisualfinish
	delay 4
	end
@ genanims: ENTROPY (T-134, vision.md 9.24) approved.
Move_EMBER:
	playsewithpan SE_M_EMBER, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 7, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 2, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 2, 7, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 7, 1, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 1, 5, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 5, 0, RGB(27, 19, 5)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_MEGA_PUNCH:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_MEGA_KICK, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
SetImpactBackground:
	delay 2
	createvisualtask AnimTask_IsContest, 2
	jumprettrue SetImpactContestsBG
	createvisualtask AnimTask_IsTargetPlayerSide, 2
	jumpretfalse SetImpactOpponentBG
	jumprettrue SetImpactPlayerBG
SetImpactBackgroundRet:
	return

SetImpactOpponentBG:
	changebg BG_IMPACT_OPPONENT
	goto SetImpactBackgroundRet

SetImpactPlayerBG:
	changebg BG_IMPACT_PLAYER
	goto SetImpactBackgroundRet

SetImpactContestsBG:
	changebg BG_IMPACT_CONTESTS
	goto SetImpactBackgroundRet

@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_MEGA_KICK:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_MEGA_KICK, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 5, 0, 7, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 14, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 14, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_COMET_PUNCH:
	playsewithpan SE_M_COMET_PUNCH, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_SONIC_BOOM:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_RAZOR_WIND2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: SIGNAL (T-134, vision.md 9.24) approved.
Move_THUNDER_SHOCK:
	playsewithpan SE_M_THUNDERBOLT, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 4, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 4
	end
@ genanims: SIGNAL (T-134, vision.md 9.24) approved.
Move_THUNDERBOLT:
	playsewithpan SE_M_THUNDERBOLT, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 6, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 4, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 6, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 13, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 13, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 4
	end
@ genanims: AFFLICT (T-134, vision.md 9.24) approved.
Move_THUNDER_WAVE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(10, 23, 23)
	waitforvisualfinish
	playsewithpan SE_M_THUNDER_WAVE, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 8, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(10, 23, 23)
	waitforvisualfinish
	end
@ genanims: OPAQUE (T-134, vision.md 9.24) approved.
Move_BEAT_UP:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 13, RGB(5, 5, 5)
	waitforvisualfinish
	playsewithpan SE_M_VITAL_THROW2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 4, 1
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 13, 0, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 13, RGB(5, 5, 5)
	waitforvisualfinish
	playsewithpan SE_M_VITAL_THROW2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 4, 1
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 13, 0, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 13, RGB(5, 5, 5)
	waitforvisualfinish
	playsewithpan SE_M_VITAL_THROW2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 4, 1
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 13, 0, RGB(5, 5, 5)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_STOMP:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_DOUBLE_TEAM, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: LOWER (T-134, vision.md 9.24) approved.
Move_TAIL_WHIP:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_TAIL_WHIP, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 5, RGB(13, 13, 13)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 5, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_CUT:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_CUT, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_CUT, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_HIDDEN_POWER:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_TAKE_DOWN, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: LOGIC (T-134, vision.md 9.24) approved.
Move_REVERSAL:
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 5, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 5, 10, RGB(12, 15, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 10, 0, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 4, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 8, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_DETECT, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(12, 15, 19)
	waitforvisualfinish
	end
@ genanims: OPAQUE (T-134, vision.md 9.24) approved.
Move_PURSUIT:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 13, RGB(5, 5, 5)
	waitforvisualfinish
	playsewithpan SE_M_COMET_PUNCH, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 4, 1
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 13, 0, RGB(5, 5, 5)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_SPIKE_CANNON:
	playsewithpan SE_M_RAZOR_WIND2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: RAISE (T-134, vision.md 9.24) approved.
Move_SWORDS_DANCE:
	playsewithpan SE_M_SWORDS_DANCE, SOUND_PAN_ATTACKER
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 6, 12, RGB(24, 23, 21)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 12, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: COPY (T-134, vision.md 9.24) approved.
Move_PSYCH_UP:
	playsewithpan SE_M_PSYBEAM2, SOUND_PAN_TARGET
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 12, RGB(24, 23, 21)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 12, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 12, RGB(24, 23, 21)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 12, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_DIZZY_PUNCH:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_COMET_PUNCH, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: ENTROPY (T-134, vision.md 9.24) approved.
Move_FIRE_SPIN:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(27, 19, 5)
	waitforvisualfinish
	playsewithpan SE_M_SACRED_FIRE2, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 8, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 1, 2, 2
	delay 8
	playsewithpan SE_M_SACRED_FIRE2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 1, 2, 2
	delay 8
	playsewithpan SE_M_SACRED_FIRE2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 1, 2, 2
	delay 8
	playsewithpan SE_M_SACRED_FIRE2, SOUND_PAN_TARGET
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 8, 0, RGB(27, 19, 5)
	waitforvisualfinish
	end
FireSpinEffect:
	createsprite gFireSpinSpriteTemplate, ANIM_TARGET, 2, 0, 28, 528, 30, 13, 50, ANIM_TARGET
	delay 2
	createsprite gFireSpinSpriteTemplate, ANIM_TARGET, 2, 0, 32, 480, 20, 16, -46, ANIM_TARGET
	delay 2
	createsprite gFireSpinSpriteTemplate, ANIM_TARGET, 2, 0, 33, 576, 20, 8, 42, ANIM_TARGET
	delay 2
	createsprite gFireSpinSpriteTemplate, ANIM_TARGET, 2, 0, 31, 400, 25, 11, -42, ANIM_TARGET
	delay 2
	createsprite gFireSpinSpriteTemplate, ANIM_TARGET, 2, 0, 28, 512, 25, 16, 46, ANIM_TARGET
	delay 2
	createsprite gFireSpinSpriteTemplate, ANIM_TARGET, 2, 0, 33, 464, 30, 15, -50, ANIM_TARGET
	delay 2
	return

@ genanims: SWARM (T-134, vision.md 9.24) approved.
Move_FURY_CUTTER:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(16, 18, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_RAZOR_WIND, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 2, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_RAZOR_WIND, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 2, 5, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_RAZOR_WIND, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 5, 9, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_RAZOR_WIND, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 15, RGB(16, 18, 9)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 15, 0, RGB(16, 18, 9)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_SELF_DESTRUCT:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 16, RGB(13, 13, 13)
	waitforvisualfinish
	playsewithpan SE_M_SELF_DESTRUCT, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 8, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_DEF_SIDE, 0, 0, 16, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_DEF_SIDE, 2, 16, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 16, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_SLAM:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_COMET_PUNCH, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: GROWTH (T-134, vision.md 9.24) approved.
Move_VINE_WHIP:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 6, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 0, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(11, 19, 12)
	waitforvisualfinish
	playsewithpan SE_M_JUMP_KICK, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 3, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 4, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 4, 0, RGB(11, 19, 12)
	waitforvisualfinish
	end
@ genanims: VECTOR (T-134, vision.md 9.24) approved.
Move_DRILL_PECK:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 8, 0, RGB(25, 8, 8)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 5, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 5, 0, RGB(25, 8, 8)
	playsewithpan SE_M_HEADBUTT, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 6, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 6, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 6, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 0, RGB(25, 8, 8)
	waitforvisualfinish
	end
@ genanims: FLOW (T-134, vision.md 9.24) approved.
Move_WATERFALL:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(8, 13, 22)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 4, RGB(8, 13, 22)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 8, RGB(8, 13, 22)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 12, RGB(8, 13, 22)
	waitforvisualfinish
	playsewithpan SE_M_CRABHAMMER, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 4, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 0, RGB(8, 13, 22)
	waitforvisualfinish
	end
RisingWaterHitEffect:
	playsewithpan SE_M_WATERFALL, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon2, 5, ANIM_TARGET, 4, 0, 17, 1
	createsprite gWaterHitSplatSpriteTemplate, ANIM_ATTACKER, 3, 0, 20, ANIM_TARGET, 1
	createsprite gSmallDriftingBubblesSpriteTemplate, ANIM_ATTACKER, 4, 0, 20
	createsprite gSmallDriftingBubblesSpriteTemplate, ANIM_ATTACKER, 4, 0, 20
	delay 2
	createsprite gWaterHitSplatSpriteTemplate, ANIM_ATTACKER, 3, 0, 15, ANIM_TARGET, 1
	createsprite gSmallDriftingBubblesSpriteTemplate, ANIM_ATTACKER, 4, 0, 15
	createsprite gSmallDriftingBubblesSpriteTemplate, ANIM_ATTACKER, 4, 0, 15
	delay 2
	createsprite gWaterHitSplatSpriteTemplate, ANIM_ATTACKER, 3, 0, 10, ANIM_TARGET, 1
	createsprite gSmallDriftingBubblesSpriteTemplate, ANIM_ATTACKER, 4, 0, 10
	createsprite gSmallDriftingBubblesSpriteTemplate, ANIM_ATTACKER, 4, 0, 10
	delay 2
	createsprite gWaterHitSplatSpriteTemplate, ANIM_ATTACKER, 3, 0, 5, ANIM_TARGET, 1
	createsprite gSmallDriftingBubblesSpriteTemplate, ANIM_ATTACKER, 4, 0, 5
	createsprite gSmallDriftingBubblesSpriteTemplate, ANIM_ATTACKER, 4, 0, 5
	delay 2
	createsprite gWaterHitSplatSpriteTemplate, ANIM_ATTACKER, 3, 0, 0, ANIM_TARGET, 1
	createsprite gSmallDriftingBubblesSpriteTemplate, ANIM_ATTACKER, 4, 0, 0
	createsprite gSmallDriftingBubblesSpriteTemplate, ANIM_ATTACKER, 4, 0, 0
	delay 2
	createsprite gWaterHitSplatSpriteTemplate, ANIM_ATTACKER, 3, 0, -5, ANIM_TARGET, 1
	createsprite gSmallDriftingBubblesSpriteTemplate, ANIM_ATTACKER, 4, 0, -5
	createsprite gSmallDriftingBubblesSpriteTemplate, ANIM_ATTACKER, 4, 0, -5
	delay 2
	createsprite gWaterHitSplatSpriteTemplate, ANIM_ATTACKER, 3, 0, -10, ANIM_TARGET, 1
	createsprite gSmallDriftingBubblesSpriteTemplate, ANIM_ATTACKER, 4, 0, -10
	createsprite gSmallDriftingBubblesSpriteTemplate, ANIM_ATTACKER, 4, 0, -10
	delay 2
	createsprite gWaterHitSplatSpriteTemplate, ANIM_ATTACKER, 3, 0, -15, ANIM_TARGET, 1
	createsprite gSmallDriftingBubblesSpriteTemplate, ANIM_ATTACKER, 4, 0, -15
	createsprite gSmallDriftingBubblesSpriteTemplate, ANIM_ATTACKER, 4, 0, -15
	delay 2
	createsprite gWaterHitSplatSpriteTemplate, ANIM_ATTACKER, 3, 0, -20, ANIM_TARGET, 1
	createsprite gSmallDriftingBubblesSpriteTemplate, ANIM_ATTACKER, 4, 0, -20
	createsprite gSmallDriftingBubblesSpriteTemplate, ANIM_ATTACKER, 4, 0, -20
	return

@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_EXPLOSION:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 16, RGB(13, 13, 13)
	waitforvisualfinish
	playsewithpan SE_M_EXPLOSION, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 8, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_DEF_SIDE, 0, 0, 16, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_DEF_SIDE, 2, 16, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 16, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: RAISE (T-134, vision.md 9.24) approved.
Move_DEFENSE_CURL:
	playsewithpan SE_M_TRI_ATTACK, SOUND_PAN_ATTACKER
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: PROTECT (T-134, vision.md 9.24) approved.
Move_PROTECT:
	playsewithpan SE_M_REFLECT, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 11, RGB(24, 23, 21)
	waitforvisualfinish
	delay 16
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 11, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: PROTECT (T-134, vision.md 9.24) approved.
Move_DETECT:
	playsewithpan SE_M_DETECT, SOUND_PAN_ATTACKER
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 11, RGB(12, 15, 19)
	waitforvisualfinish
	delay 16
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 11, 0, RGB(12, 15, 19)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_FRUSTRATION:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_DRAGON_RAGE, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_SAFEGUARD:
	playsewithpan SE_M_MILK_DRINK, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: COPY (T-134, vision.md 9.24) approved.
Move_PAIN_SPLIT:
	playsewithpan SE_M_SWAGGER2, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(24, 23, 21)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 2, RGB(24, 23, 21)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 12, 7, RGB(24, 23, 21)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 2, 7, RGB(24, 23, 21)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER | F_PAL_TARGET, 1, 7, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_VICE_GRIP:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_VICEGRIP, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 4, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_GUILLOTINE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_VICEGRIP, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 6, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 16, RGB_BLACK
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 16, 0, RGB_BLACK
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_PAY_DAY:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_RAZOR_WIND2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 4, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(24, 23, 21)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: EMERGENT (T-134, vision.md 9.24) approved.
Move_OUTRAGE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 3, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 3, 0, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 3, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 0, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 3, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 3, 0, RGB(6, 15, 13)
	waitforvisualfinish
	playsewithpan SE_M_DRAGON_RAGE, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 6, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_DEF_SIDE, 0, 0, 9, RGB(6, 15, 13)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 13, RGB(6, 15, 13)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_DEF_SIDE, 1, 9, 0, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 13, 0, RGB(6, 15, 13)
	waitforvisualfinish
	playsewithpan SE_M_DRAGON_RAGE, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 6, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_BATTLERS, 0, 0, 9, RGB(6, 15, 13)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 13, RGB(6, 15, 13)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_BATTLERS, 1, 9, 0, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 13, 0, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: SIGNAL (T-134, vision.md 9.24) approved.
Move_SPARK:
	playsewithpan SE_M_THUNDERBOLT2, SOUND_PAN_TARGET
	playsewithpan SE_M_THUNDERBOLT2, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 11, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 11, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_THUNDERBOLT2, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_THUNDERBOLT2, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 11, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 11, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_THUNDERBOLT2, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_THUNDERBOLT2, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 11, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 11, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_THUNDERBOLT2, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 2
	end
@ genanims: AFFLICT (T-134, vision.md 9.24) approved.
Move_ATTRACT:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_CHARM, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER | F_PAL_TARGET, 1, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER | F_PAL_TARGET, 1, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER | F_PAL_TARGET, 1, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER | F_PAL_TARGET, 1, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: RAISE (T-134, vision.md 9.24) approved.
Move_GROWTH:
	playsewithpan SE_M_TAKE_DOWN, SOUND_PAN_ATTACKER
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_WHIRLWIND:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_DOUBLE_TEAM, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_SlideOffScreen, 5, ANIM_TARGET, 8
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: AFFLICT (T-134, vision.md 9.24) approved.
Move_CONFUSE_RAY:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(11, 8, 14)
	waitforvisualfinish
	playsewithpan SE_M_STRING_SHOT2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 6, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_LOCK_ON:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_COMET_PUNCH, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 9, RGB_WHITE
	waitforvisualfinish
	delay 20
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 9, 0, RGB_WHITE
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_MEAN_LOOK:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_PSYBEAM, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(13, 13, 13)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 12, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: LEGACY (T-134, vision.md 9.24) approved.
Move_ROCK_THROW:
	playsewithpan SE_M_ROCK_THROW, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 1, 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 6, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 3, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 0, RGB(26, 26, 26)
	waitforvisualfinish
	end
@ genanims: LEGACY (T-134, vision.md 9.24) approved.
Move_ROCK_SLIDE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 0, 7, RGB(26, 26, 26)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 7, 0, RGB(26, 26, 26)
	waitforvisualfinish
	delay 6
	playsewithpan SE_M_ROCK_THROW, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 2
	createvisualtask AnimTask_ShakeMon, 2, ANIM_DEF_PARTNER, 3, 0, 5, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 0, 12, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 12, 8, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 8, 4, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 4, 0, RGB(26, 26, 26)
	waitforvisualfinish
	end
@ genanims: OPAQUE (T-134, vision.md 9.24) approved.
Move_THIEF:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 13, RGB(5, 5, 5)
	waitforvisualfinish
	playsewithpan SE_M_VITAL_THROW, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 4, 1
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 13, 0, RGB(5, 5, 5)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 8, 0, RGB(5, 5, 5)
	waitforvisualfinish
	end
@ genanims: FLOW (T-134, vision.md 9.24) approved.
Move_BUBBLE_BEAM:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(8, 13, 22)
	waitforvisualfinish
	playsewithpan SE_M_BUBBLE, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 0, RGB(8, 13, 22)
	waitforvisualfinish
	playsewithpan SE_M_BUBBLE, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(8, 13, 22)
	waitforvisualfinish
	playsewithpan SE_M_BUBBLE, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 5, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 5, 0, RGB(8, 13, 22)
	waitforvisualfinish
	end
@ genanims: FROZEN (T-134, vision.md 9.24) approved.
Move_ICY_WIND:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(20, 25, 27)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(20, 25, 27)
	waitforvisualfinish
	playsewithpan SE_M_ICY_WIND, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 0, 10, RGB(20, 25, 27)
	waitforvisualfinish
	delay 24
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 2, 10, 0, RGB(20, 25, 27)
	waitforvisualfinish
	end
@ genanims: LOWER (T-134, vision.md 9.24) approved.
Move_SMOKESCREEN:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_DOUBLE_TEAM, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET | F_PAL_BG, 1, 0, 5, RGB(13, 13, 13)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET | F_PAL_BG, 1, 5, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: COPY (T-134, vision.md 9.24) approved.
Move_CONVERSION:
	playsewithpan SE_M_SWIFT, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 10, RGB(13, 13, 13)
	waitforvisualfinish
	delay 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 10, 0, RGB(13, 13, 13)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_DaemonsBlendToUserType, 10, 0, 0, 12
	waitforvisualfinish
	delay 12
	createvisualtask AnimTask_DaemonsBlendToUserType, 10, 1, 12, 0
	waitforvisualfinish
	end
@ genanims: COPY (T-134, vision.md 9.24) approved.
Move_CONVERSION_2:
	playsewithpan SE_M_BARRIER, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	delay 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 0, 12, RGB(13, 13, 13)
	waitforvisualfinish
	delay 30
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 12, 0, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_DaemonsBlendToUserType, 10, 1, 0, 13
	waitforvisualfinish
	delay 16
	createvisualtask AnimTask_DaemonsBlendToUserType, 10, 2, 13, 0
	waitforvisualfinish
	end
@ genanims: LOGIC (T-134, vision.md 9.24) approved.
Move_ROLLING_KICK:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(12, 15, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 6, RGB(12, 15, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 0, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 6, RGB(12, 15, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 0, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 3, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 6, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 9, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_DOUBLE_TEAM, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 1
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(12, 15, 19)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_HEADBUTT:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_HEADBUTT, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_HORN_ATTACK:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_HEADBUTT, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_FURY_ATTACK:
	playsewithpan SE_M_HORN_ATTACK, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_HORN_DRILL:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_HEADBUTT, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 6, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 16, RGB_BLACK
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 16, 0, RGB_BLACK
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_THRASH:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_COMET_PUNCH, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 6, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 13, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 13, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_COMET_PUNCH, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 6, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 13, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 13, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: AFFLICT (T-134, vision.md 9.24) approved.
Move_SING:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_SING, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 3, 0, 12, RGB(13, 13, 13)
	waitforvisualfinish
	delay 16
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 12, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: LOGIC (T-134, vision.md 9.24) approved.
Move_LOW_KICK:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(12, 15, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 4, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 8, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_VITAL_THROW2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(12, 15, 19)
	waitforvisualfinish
	end
@ genanims: STRATUM (T-134, vision.md 9.24) approved.
Move_EARTHQUAKE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(19, 15, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(19, 15, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 8, RGB(19, 15, 9)
	waitforvisualfinish
	playsewithpan SE_M_EARTHQUAKE, SOUND_PAN_TARGET
	createvisualtask AnimTask_HorizontalShake, 5, (MAX_BATTLERS_COUNT + 1), 7, 20
	createvisualtask AnimTask_HorizontalShake, 5, MAX_BATTLERS_COUNT, 7, 20
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BATTLERS, 0, 0, 13, RGB(19, 15, 9)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BATTLERS, 1, 13, 0, RGB(19, 15, 9)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 8, 0, RGB(19, 15, 9)
	waitforvisualfinish
	end
@ genanims: STRATUM (T-134, vision.md 9.24) approved.
Move_FISSURE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 10, RGB(19, 15, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 10, 0, RGB(19, 15, 9)
	waitforvisualfinish
	playsewithpan SE_M_EARTHQUAKE, SOUND_PAN_TARGET
	createvisualtask AnimTask_HorizontalShake, 5, (MAX_BATTLERS_COUNT + 1), 5, 16
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 0, 14, RGB_BLACK
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 16, RGB_BLACK
	waitforvisualfinish
	delay 12
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 16, 0, RGB_BLACK
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 2, 14, 0, RGB_BLACK
	waitforvisualfinish
	end
@ genanims: STRATUM (T-134, vision.md 9.24) approved.
Move_DIG:
	choosetwoturnanim DaemonsExcavateUnder, DaemonsExcavateUp
DaemonsExcavateDone:
	end
DaemonsExcavateUnder:
	playsewithpan SE_M_MEGA_KICK2, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_ShakeMon, 2, ANIM_ATTACKER, 0, 2, 6, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 12, RGB(19, 15, 9)
	waitforvisualfinish
	invisible ANIM_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 12, 0, RGB(19, 15, 9)
	waitforvisualfinish
	goto DaemonsExcavateDone
DaemonsExcavateUp:
	visible ANIM_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 5, RGB(19, 15, 9)
	waitforvisualfinish
	playsewithpan SE_M_MEGA_KICK2, SOUND_PAN_TARGET
	createvisualtask AnimTask_HorizontalShake, 5, (MAX_BATTLERS_COUNT + 1), 2, 8
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 5, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(19, 15, 9)
	waitforvisualfinish
	delay 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 11, 0, RGB(19, 15, 9)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 5, 0, RGB(19, 15, 9)
	waitforvisualfinish
	goto DaemonsExcavateDone
	end
@ genanims: RAISE (T-134, vision.md 9.24) approved.
Move_MEDITATE:
	playsewithpan SE_M_HEADBUTT, SOUND_PAN_ATTACKER
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(22, 10, 19)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 6, 0, RGB(22, 10, 19)
	waitforvisualfinish
	end
@ genanims: RAISE (T-134, vision.md 9.24) approved.
Move_AGILITY:
	playsewithpan SE_M_DOUBLE_TEAM, SOUND_PAN_ATTACKER
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(22, 10, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 12, RGB(22, 10, 19)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 12, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(22, 10, 19)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_QUICK_ATTACK:
	playsewithpan SE_M_JUMP_KICK, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 4, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_RAGE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_SWAGGER2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_SWAGGER2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_TELEPORT:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 12, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_Teleport, 2
	playsewithpan SE_M_TELEPORT, SOUND_PAN_ATTACKER
	delay 15
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 12, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: RAISE (T-134, vision.md 9.24) approved.
Move_DOUBLE_TEAM:
	playsewithpan SE_M_DOUBLE_TEAM, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: RAISE (T-134, vision.md 9.24) approved.
Move_MINIMIZE:
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: COPY (T-134, vision.md 9.24) approved.
Move_METRONOME:
	playsewithpan SE_M_METRONOME, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 9, RGB(24, 23, 21)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 9, 9, RGB(24, 23, 21)
	waitforvisualfinish
	delay 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 3, RGB(24, 23, 21)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 3, 3, RGB(24, 23, 21)
	waitforvisualfinish
	delay 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 12, RGB(24, 23, 21)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 12, 12, RGB(24, 23, 21)
	waitforvisualfinish
	delay 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 6, RGB(24, 23, 21)
	waitforvisualfinish
	delay 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 10, RGB(24, 23, 21)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 10, 10, RGB(24, 23, 21)
	waitforvisualfinish
	delay 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_SKULL_BASH:
	choosetwoturnanim DaemonsSkullBashLoad, DaemonsSkullBashRun
DaemonsSkullBashDone:
	end
DaemonsSkullBashLoad:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	delay 12
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	goto DaemonsSkullBashDone
DaemonsSkullBashRun:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_TAKE_DOWN, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 6, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 13, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 13, 0, RGB(24, 23, 21)
	waitforvisualfinish
	goto DaemonsSkullBashDone
	end
@ genanims: RAISE (T-134, vision.md 9.24) approved.
Move_AMNESIA:
	playsewithpan SE_M_METRONOME, SOUND_PAN_ATTACKER
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(22, 10, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 6, 12, RGB(22, 10, 19)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 12, 0, RGB(22, 10, 19)
	waitforvisualfinish
	end
@ genanims: LOWER (T-134, vision.md 9.24) approved.
Move_KINESIS:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(22, 10, 19)
	waitforvisualfinish
	playsewithpan SE_M_PSYBEAM, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(22, 10, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET | F_PAL_BG, 1, 0, 5, RGB(13, 13, 13)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET | F_PAL_BG, 1, 5, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: AFFLICT (T-134, vision.md 9.24) approved.
Move_GLARE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_PSYBEAM2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 8, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_BARRAGE:
	playsewithpan SE_M_SWAGGER, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: VECTOR (T-134, vision.md 9.24) approved.
Move_SKY_ATTACK:
	choosetwoturnanim DaemonsBallisticAim, DaemonsBallisticFire
DaemonsBallisticDone:
	end
DaemonsBallisticAim:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 0, 12, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_ShakeMon, 2, ANIM_ATTACKER, 1, 0, 6, 2
	delay 16
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 12, 0, RGB(25, 8, 8)
	waitforvisualfinish
	goto DaemonsBallisticDone
DaemonsBallisticFire:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 8, 0, RGB(25, 8, 8)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 5, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 5, 0, RGB(25, 8, 8)
	playsewithpan SE_M_STAT_INCREASE, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 5, 0, 7, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 14, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 14, 0, RGB(25, 8, 8)
	waitforvisualfinish
	goto DaemonsBallisticDone
	end
@ genanims: LOWER (T-134, vision.md 9.24) approved.
Move_FLASH:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_LEER, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET | F_PAL_BG, 1, 0, 5, RGB(13, 13, 13)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET | F_PAL_BG, 1, 5, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_SPLASH:
	playsewithpan SE_M_TAIL_WHIP, SOUND_PAN_ATTACKER
	delay 20
	end
@ genanims: RAISE (T-134, vision.md 9.24) approved.
Move_ACID_ARMOR:
	playsewithpan SE_M_ACID_ARMOR, SOUND_PAN_ATTACKER
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(10, 11, 6)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 6, 12, RGB(10, 11, 6)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 12, 0, RGB(10, 11, 6)
	waitforvisualfinish
	end
@ genanims: RAISE (T-134, vision.md 9.24) approved.
Move_SHARPEN:
	playsewithpan SE_M_COMET_PUNCH, SOUND_PAN_ATTACKER
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_SUPER_FANG:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_DRAGON_RAGE, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	delay 12
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_SLASH:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_RAZOR_WIND, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_RAZOR_WIND, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_STRUGGLE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_HEADBUTT, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 4, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: COPY (T-134, vision.md 9.24) approved.
Move_SKETCH:
	playsewithpan SE_M_TAIL_WHIP, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	delay 20
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_NIGHTMARE:
	playsewithpan SE_M_NIGHTMARE, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 12, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 8, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 12, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 8, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 12, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 8, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_FLAIL:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_HEADBUTT, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_SPITE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(11, 8, 14)
	waitforvisualfinish
	playsewithpan SE_M_PSYBEAM, SOUND_PAN_TARGET
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 4, RGB(13, 13, 13)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 4, 8, RGB(13, 13, 13)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 8, 12, RGB(13, 13, 13)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 12, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: LOGIC (T-134, vision.md 9.24) approved.
Move_MACH_PUNCH:
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 4, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 8, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_JUMP_KICK, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 4, 1
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(12, 15, 19)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_FORESIGHT:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_SKETCH, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 9, RGB_WHITE
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 9, 0, RGB_WHITE
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_DESTINY_BOND:
	playsewithpan SE_M_PSYBEAM, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER | F_PAL_TARGET, 1, 0, 10, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER | F_PAL_TARGET, 1, 10, 0, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER | F_PAL_TARGET, 1, 0, 10, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER | F_PAL_TARGET, 1, 10, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: PROTECT (T-134, vision.md 9.24) approved.
Move_ENDURE:
	playsewithpan SE_M_DRAGON_RAGE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 13, RGB(13, 13, 13)
	waitforvisualfinish
	delay 18
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 13, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
EndureEffect:
	createsprite gEndureEnergySpriteTemplate, ANIM_ATTACKER, 2, 0, -24, 26, 2
	delay 4
	createsprite gEndureEnergySpriteTemplate, ANIM_ATTACKER, 2, 0, 14, 28, 1
	delay 4
	createsprite gEndureEnergySpriteTemplate, ANIM_ATTACKER, 2, 0, -5, 10, 2
	delay 4
	createsprite gEndureEnergySpriteTemplate, ANIM_ATTACKER, 2, 0, 28, 26, 3
	delay 4
	createsprite gEndureEnergySpriteTemplate, ANIM_ATTACKER, 2, 0, -12, 0, 1
	return

@ genanims: LOWER (T-134, vision.md 9.24) approved.
Move_CHARM:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_CHARM, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 5, RGB(13, 13, 13)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 5, 10, RGB(13, 13, 13)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 10, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: LEGACY (T-134, vision.md 9.24) approved.
Move_ROLLOUT:
	createvisualtask AnimTask_GetRolloutCounter, 5, 0
	jumpargeq 0, 1, DaemonsRotate1
	jumpargeq 0, 2, DaemonsRotate2
	jumpargeq 0, 3, DaemonsRotate3
	jumpargeq 0, 4, DaemonsRotate4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 0, 7, RGB(26, 26, 26)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 7, 0, RGB(26, 26, 26)
	waitforvisualfinish
	delay 6
	playsewithpan SE_M_MEGA_KICK2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 4, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 6, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 4, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 2, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 2, 0, RGB(26, 26, 26)
	waitforvisualfinish
	end
DaemonsRotate1:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 0, 7, RGB(26, 26, 26)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 7, 0, RGB(26, 26, 26)
	waitforvisualfinish
	delay 6
	playsewithpan SE_M_MEGA_KICK2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 4, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 5, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 5, 2, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 2, 0, RGB(26, 26, 26)
	waitforvisualfinish
	end
DaemonsRotate2:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 0, 7, RGB(26, 26, 26)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 7, 0, RGB(26, 26, 26)
	waitforvisualfinish
	delay 6
	playsewithpan SE_M_MEGA_KICK2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 4, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 6, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 3, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 0, RGB(26, 26, 26)
	waitforvisualfinish
	end
DaemonsRotate3:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 0, 7, RGB(26, 26, 26)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 7, 0, RGB(26, 26, 26)
	waitforvisualfinish
	delay 6
	playsewithpan SE_M_MEGA_KICK2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 4, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 8, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 4, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 0, RGB(26, 26, 26)
	waitforvisualfinish
	end
DaemonsRotate4:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 0, 7, RGB(26, 26, 26)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 7, 0, RGB(26, 26, 26)
	waitforvisualfinish
	delay 6
	playsewithpan SE_M_MEGA_KICK2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 5, 0, 4, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 14, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 14, 9, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 4, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 0, RGB(26, 26, 26)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_FALSE_SWIPE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_VITAL_THROW, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: AFFLICT (T-134, vision.md 9.24) approved.
Move_SWAGGER:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_SWAGGER, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 6, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ DAEMONS -- REFILL -- filled, not reloaded: the colour drains, then comes back in ONE slow continuous pour where RESTORE
@ steps.
@ T-139, approved 2026-09-17 under vision.md 9.24.
Move_MILK_DRINK:
	monbg ANIM_ATTACKER
	playsewithpan SE_M_TELEPORT, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 13, RGB(13, 13, 13)
	waitforvisualfinish
	delay 8
	playsewithpan SE_M_MILK_DRINK, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 4, 13, 0, RGB(13, 13, 13)
	waitforvisualfinish
	clearmonbg ANIM_ATTACKER
	end

@ genanims: STRATUM (T-134, vision.md 9.24) approved.
Move_MAGNITUDE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(19, 15, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(19, 15, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 8, RGB(19, 15, 9)
	waitforvisualfinish
	playsewithpan SE_M_STRENGTH, SOUND_PAN_TARGET
	createvisualtask AnimTask_HorizontalShake, 5, (MAX_BATTLERS_COUNT + 1), 6, 20
	createvisualtask AnimTask_HorizontalShake, 5, MAX_BATTLERS_COUNT, 6, 20
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BATTLERS, 0, 0, 11, RGB(19, 15, 9)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BATTLERS, 1, 11, 0, RGB(19, 15, 9)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 8, 0, RGB(19, 15, 9)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_RAPID_SPIN:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 7, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 7, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 7, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 7, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 7, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 7, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_RAZOR_WIND2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ DAEMONS -- NIGHT REPAIR -- maintenance run while the field is dark: the ground dims to black (neutral, not colour),
@ the repair happens in two quiet steps, and the light comes back. No moon, no sparkles.
@ T-139, approved 2026-09-17 under vision.md 9.24.
Move_MOONLIGHT:
	monbg ANIM_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 0, 12, RGB_BLACK
	waitforvisualfinish
	playsewithpan SE_M_MOONLIGHT, 0
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 11, RGB(13, 13, 13)
	waitforvisualfinish
	delay 12
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 11, 5, RGB(13, 13, 13)
	waitforvisualfinish
	delay 12
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 5, 0, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 12, 0, RGB_BLACK
	waitforvisualfinish
	clearmonbg ANIM_ATTACKER
	end

@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_EXTREME_SPEED:
	playsewithpan SE_M_RAZOR_WIND2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_RAZOR_WIND2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_UPROAR:
	playsewithpan SE_M_UPROAR, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_DEF_SIDE, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_DEF_SIDE, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_UPROAR, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_DEF_SIDE, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_DEF_SIDE, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: ENTROPY (T-134, vision.md 9.24) approved.
Move_HEAT_WAVE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(27, 19, 5)
	waitforvisualfinish
	playsewithpan SE_M_HEAT_WAVE, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 9, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 0, 13, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 13, 3, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 3, 10, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 10, 1, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 1, 7, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 7, 4, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 4, 11, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 11, 2, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 2, 6, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 6, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 6, 2, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 2, 5, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 5, 1, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 1, 4, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 4, 0, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 1, 6, 0, RGB(27, 19, 5)
	waitforvisualfinish
	end
@ Also used by Hail weather
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_HAIL:
	playsewithpan SE_M_HAIL, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 0, 9, RGB(20, 25, 27)
	waitforvisualfinish
	delay 12
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 9, 0, RGB(20, 25, 27)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_TORMENT:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(5, 5, 5)
	waitforvisualfinish
	playsewithpan SE_M_SWAGGER2, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(5, 5, 5)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(13, 13, 13)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 12, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_MEMENTO:
	playsewithpan SE_M_PSYBEAM, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 16, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 16, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_FACADE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_SWAGGER, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_SMELLING_SALT:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_DOUBLE_SLAP, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: PROTECT (T-134, vision.md 9.24) approved.
Move_FOLLOW_ME:
	playsewithpan SE_M_TAIL_WHIP, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_DEF_SIDE, 1, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_DEF_SIDE, 1, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_CHARGE:
	playsewithpan SE_M_CHARGE, SOUND_PAN_ATTACKER
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(10, 23, 23)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 6, 0, RGB(10, 23, 23)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_TAUNT:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(5, 5, 5)
	waitforvisualfinish
	playsewithpan SE_M_METRONOME, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(5, 5, 5)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(13, 13, 13)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 12, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: PROTECT (T-134, vision.md 9.24) approved.
Move_HELPING_HAND:
	playsewithpan SE_M_ENCORE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 0, 0, 9, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 9, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: COPY (T-134, vision.md 9.24) approved.
Move_ASSIST:
	playsewithpan SE_M_SCRATCH, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 0, 0, 9, RGB(24, 23, 21)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 0, 9, 9, RGB(24, 23, 21)
	waitforvisualfinish
	delay 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 0, 0, 3, RGB(24, 23, 21)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 0, 3, 3, RGB(24, 23, 21)
	waitforvisualfinish
	delay 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 0, 0, 12, RGB(24, 23, 21)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 0, 12, 12, RGB(24, 23, 21)
	waitforvisualfinish
	delay 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 0, 0, 6, RGB(24, 23, 21)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 0, 6, 6, RGB(24, 23, 21)
	waitforvisualfinish
	delay 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 0, 0, 10, RGB(24, 23, 21)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 0, 10, 10, RGB(24, 23, 21)
	waitforvisualfinish
	delay 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 0, 0, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: LOGIC (T-134, vision.md 9.24) approved.
Move_SUPERPOWER:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(12, 15, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 3, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 6, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 9, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 12, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MEGA_KICK, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 5, 0, 7, 1
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 0, RGB(12, 15, 19)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: COPY (T-134, vision.md 9.24) approved.
Move_RECYCLE:
	playsewithpan SE_M_TAKE_DOWN, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 9, RGB(24, 23, 21)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 9, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 9, RGB(24, 23, 21)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 9, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: LOGIC (T-134, vision.md 9.24) approved.
Move_BRICK_BREAK:
	choosetwoturnanim DaemonsFalsifyPlain, DaemonsFalsifyScreen
DaemonsFalsifyDone:
	end
DaemonsFalsifyPlain:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(12, 15, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 4, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 8, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 12, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_VITAL_THROW, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 1
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 0, RGB(12, 15, 19)
	waitforvisualfinish
	goto DaemonsFalsifyDone
DaemonsFalsifyScreen:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 0, 10, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 0, 12, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 1, 12, 0, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(12, 15, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 4, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 8, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 12, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_VITAL_THROW, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 1
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 0, RGB(12, 15, 19)
	waitforvisualfinish
	goto DaemonsFalsifyDone
	end
@ genanims: AFFLICT (T-134, vision.md 9.24) approved.
Move_YAWN:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_YAWN, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 4, 0, 7, RGB(13, 13, 13)
	waitforvisualfinish
	delay 16
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 7, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_ENDEAVOR:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_TAIL_WHIP, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER | F_PAL_TARGET, 1, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	delay 12
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER | F_PAL_TARGET, 1, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: ENTROPY (T-134, vision.md 9.24) approved.
Move_ERUPTION:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(27, 19, 5)
	waitforvisualfinish
	playsewithpan SE_M_EXPLOSION, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 10, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 0, 14, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 14, 4, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 4, 11, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 11, 2, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 2, 8, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 8, 4, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 4, 12, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 12, 2, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 2, 7, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_ATK_SIDE, 0, 0, 6, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_ATK_SIDE, 0, 6, 2, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_ATK_SIDE, 0, 2, 5, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_ATK_SIDE, 0, 5, 1, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_ATK_SIDE, 0, 1, 4, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_ATK_SIDE, 0, 4, 0, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 1, 7, 0, RGB(27, 19, 5)
	waitforvisualfinish
	end
@ genanims: COPY (T-134, vision.md 9.24) approved.
Move_SKILL_SWAP:
	playsewithpan SE_M_REVERSAL, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER | F_PAL_TARGET, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER | F_PAL_TARGET, 0, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	playsewithpan SE_M_REVERSAL, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER | F_PAL_TARGET, 0, 0, 10, RGB(22, 10, 19)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER | F_PAL_TARGET, 0, 10, 0, RGB(22, 10, 19)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_IMPRISON:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(22, 10, 19)
	waitforvisualfinish
	playsewithpan SE_M_HORN_ATTACK, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(22, 10, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(13, 13, 13)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 12, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_GRUDGE:
	playsewithpan SE_M_PSYBEAM, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER | F_PAL_TARGET, 1, 0, 10, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER | F_PAL_TARGET, 1, 10, 0, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER | F_PAL_TARGET, 1, 0, 10, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER | F_PAL_TARGET, 1, 10, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_CAMOUFLAGE:
	monbg ANIM_ATK_PARTNER
	splitbgprio ANIM_ATTACKER
	setalpha 16, 0
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 12, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_AttackerFadeToInvisible, 2, 4
	playsewithpan SE_M_FAINT_ATTACK, SOUND_PAN_ATTACKER
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 12, 0, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_AttackerFadeFromInvisible, 2, 1
	waitforvisualfinish
	blendoff
	clearmonbg ANIM_ATK_PARTNER
	end
@ genanims: RAISE (T-134, vision.md 9.24) approved.
Move_TAIL_GLOW:
	playsewithpan SE_M_MORNING_SUN, SOUND_PAN_ATTACKER
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 6, 12, RGB(16, 18, 9)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 12, 0, RGB(16, 18, 9)
	waitforvisualfinish
	end
@ genanims: CONTEXT (T-134, vision.md 9.24) approved.
Move_LUSTER_PURGE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 0, 9, RGB(22, 10, 19)
	waitforvisualfinish
	playsewithpan SE_M_SOLAR_BEAM, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 5, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 11, RGB(22, 10, 19)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 11, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 9, 0, RGB(22, 10, 19)
	waitforvisualfinish
	end
@ genanims: CONTEXT (T-134, vision.md 9.24) approved.
Move_MIST_BALL:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 0, 9, RGB(22, 10, 19)
	waitforvisualfinish
	playsewithpan SE_M_STRING_SHOT, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 5, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 11, RGB(22, 10, 19)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 11, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 9, 0, RGB(22, 10, 19)
	waitforvisualfinish
	end
@ genanims: LOWER (T-134, vision.md 9.24) approved.
Move_FEATHER_DANCE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(25, 8, 8)
	waitforvisualfinish
	playsewithpan SE_M_PETAL_DANCE, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(25, 8, 8)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 5, RGB(13, 13, 13)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 5, 10, RGB(13, 13, 13)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 10, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: AFFLICT (T-134, vision.md 9.24) approved.
Move_TEETER_DANCE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_TEETER_DANCE, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 6, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 0, 9, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 9, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 0, 9, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 9, 0, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 0, 9, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 9, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 0, 9, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 9, 0, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 0, 9, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 9, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 0, 9, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 9, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_MUD_SPORT:
	playsewithpan SE_M_DIG, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 0, 8, RGB(19, 15, 9)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 8, 0, RGB(19, 15, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 0, 8, RGB(19, 15, 9)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 8, 0, RGB(19, 15, 9)
	waitforvisualfinish
	end
@ genanims: GROWTH (T-134, vision.md 9.24) approved.
Move_NEEDLE_ARM:
	playsewithpan SE_M_HORN_ATTACK, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 4, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 3, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 5, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 5, 0, RGB(11, 19, 12)
	waitforvisualfinish
	end
@ DAEMONS -- SLACK OFF -- recovering by doing less: the sprite slumps (vanilla's squish, which is a sag, not an effort),
@ its colour sags half-way to grey with it, and drifts back.
@ T-139, approved 2026-09-17 under vision.md 9.24.
Move_SLACK_OFF:
	monbg ANIM_ATTACKER
	createvisualtask AnimTask_SlackOffSquish, 2, ANIM_ATTACKER
	playsewithpan SE_M_YAWN, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 3, 0, 7, RGB(13, 13, 13)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 3, 7, 0, RGB(13, 13, 13)
	waitforvisualfinish
	clearmonbg ANIM_ATTACKER
	end

@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_CRUSH_CLAW:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_RAZOR_WIND, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ DAEMONS -- RETRAIN -- CLEAR STATE for the whole side, in GROWTH: everyone on the user's side goes to its type colour and
@ comes back at once, twice. No flowers.
@ T-139, approved 2026-09-17 under vision.md 9.24.
Move_AROMATHERAPY:
	playsewithpan SE_M_PETAL_DANCE, 0
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 0, 0, 10, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 10, 0, RGB(11, 19, 12)
	waitforvisualfinish
	delay 6
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 0, 0, 10, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 10, 0, RGB(11, 19, 12)
	waitforvisualfinish
	end

@ genanims: LOWER (T-134, vision.md 9.24) approved.
Move_FAKE_TEARS:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(5, 5, 5)
	waitforvisualfinish
	playsewithpan SE_M_TAIL_WHIP, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(5, 5, 5)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 5, RGB(13, 13, 13)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 5, 10, RGB(13, 13, 13)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 10, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: VECTOR (T-134, vision.md 9.24) approved.
Move_AIR_CUTTER:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 8, 0, RGB(25, 8, 8)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 5, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 5, 0, RGB(25, 8, 8)
	playsewithpan SE_M_CUT, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 4, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 0, 10, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 10, 0, RGB(25, 8, 8)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_ODOR_SLEUTH:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_SWAGGER, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 9, RGB_WHITE
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 9, 0, RGB_WHITE
	waitforvisualfinish
	end
@ genanims: AFFLICT (T-134, vision.md 9.24) approved.
Move_GRASS_WHISTLE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(11, 19, 12)
	waitforvisualfinish
	playsewithpan SE_M_GRASSWHISTLE, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 6, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 0, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 3, 0, 12, RGB(13, 13, 13)
	waitforvisualfinish
	delay 16
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 12, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_TICKLE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_DETECT, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 5, RGB(13, 13, 13)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 5, 0, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_DETECT, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 5, RGB(13, 13, 13)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 5, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: FLOW (T-134, vision.md 9.24) approved.
Move_WATER_SPOUT:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 0, 6, RGB(8, 13, 22)
	waitforvisualfinish
	playsewithpan SE_M_HEADBUTT, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 5, 7, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 0, 14, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 3, 14, 0, RGB(8, 13, 22)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 3, 6, 0, RGB(8, 13, 22)
	waitforvisualfinish
	end
@ genanims: LATENT (T-134, vision.md 9.24) approved.
Move_SHADOW_PUNCH:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 0, 3, RGB(11, 8, 14)
	waitforvisualfinish
	playsewithpan SE_M_JUMP_KICK, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 11, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 0, RGB(11, 8, 14)
	waitforvisualfinish
	end
@ genanims: CONTEXT (T-134, vision.md 9.24) approved.
Move_EXTRASENSORY:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 0, 9, RGB(22, 10, 19)
	waitforvisualfinish
	playsewithpan SE_M_BIND, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 12, RGB(22, 10, 19)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 12, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 9, 0, RGB(22, 10, 19)
	waitforvisualfinish
	end
@ genanims: VECTOR (T-134, vision.md 9.24) approved.
Move_AERIAL_ACE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 8, 0, RGB(25, 8, 8)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 5, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 5, 0, RGB(25, 8, 8)
	playsewithpan SE_M_RAZOR_WIND2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 0, RGB(25, 8, 8)
	waitforvisualfinish
	end
@ genanims: RAISE (T-134, vision.md 9.24) approved.
Move_IRON_DEFENSE:
	playsewithpan SE_SHINY, SOUND_PAN_ATTACKER
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(24, 21, 12)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 6, 12, RGB(24, 21, 12)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 12, 0, RGB(24, 21, 12)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_BLOCK:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_SWAGGER, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(13, 13, 13)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 12, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: RAISE (T-134, vision.md 9.24) approved.
Move_HOWL:
	playsewithpan SE_M_COMET_PUNCH, SOUND_PAN_ATTACKER
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: RAISE (T-134, vision.md 9.24) approved.
Move_BULK_UP:
	playsewithpan SE_M_SWAGGER, SOUND_PAN_ATTACKER
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(12, 15, 19)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 6, 0, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(12, 15, 19)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 6, 0, RGB(12, 15, 19)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_COVET:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_CHARM, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 4, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(24, 23, 21)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: SIGNAL (T-134, vision.md 9.24) approved.
Move_VOLT_TACKLE:
	playsewithpan SE_M_CHARGE, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 5, 0, 7, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 14, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 14, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 14, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 14, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 14, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 14, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 14, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 14, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 4
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_WATER_SPORT:
	playsewithpan SE_M_SURF, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 0, 8, RGB(8, 13, 22)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 8, 0, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 0, 8, RGB(8, 13, 22)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 8, 0, RGB(8, 13, 22)
	waitforvisualfinish
	end
@ genanims: RAISE (T-134, vision.md 9.24) approved.
Move_CALM_MIND:
	playsewithpan SE_M_SUPERSONIC, SOUND_PAN_ATTACKER
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(22, 10, 19)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 6, 0, RGB(22, 10, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(22, 10, 19)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 6, 0, RGB(22, 10, 19)
	waitforvisualfinish
	end
@ genanims: GROWTH (T-134, vision.md 9.24) approved.
Move_LEAF_BLADE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(11, 19, 12)
	waitforvisualfinish
	playsewithpan SE_M_CUT, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 4, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 3, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 7, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 7, 5, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 5, 0, RGB(11, 19, 12)
	waitforvisualfinish
	end
@ genanims: RAISE (T-134, vision.md 9.24) approved.
Move_DRAGON_DANCE:
	playsewithpan SE_M_TELEPORT, SOUND_PAN_ATTACKER
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(6, 15, 13)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 6, 0, RGB(6, 15, 13)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(6, 15, 13)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 6, 0, RGB(6, 15, 13)
	waitforvisualfinish
	end
@ genanims: SIGNAL (T-134, vision.md 9.24) approved.
Move_SHOCK_WAVE:
	playsewithpan SE_M_CHARGE, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 2
	end
@ genanims: RAISE (T-134, vision.md 9.24) approved.
Move_HARDEN:
	playsewithpan SE_M_HARDEN, SOUND_PAN_ATTACKER
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: COPY (T-134, vision.md 9.24) approved.
Move_BELLY_DRUM:
	playsewithpan SE_M_BELLY_DRUM, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 10, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 10, 0, RGB(13, 13, 13)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 4, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 4, 8, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 8, 12, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 12, 16, RGB(24, 23, 21)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 16, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_MIND_READER:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_CONFUSE_RAY, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 9, RGB_WHITE
	waitforvisualfinish
	delay 20
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 9, 0, RGB_WHITE
	waitforvisualfinish
	end
@ genanims: FROZEN (T-134, vision.md 9.24) approved.
Move_ICE_PUNCH:
	playsewithpan SE_M_STRING_SHOT, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(20, 25, 27)
	waitforvisualfinish
	delay 14
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 0, RGB(20, 25, 27)
	waitforvisualfinish
	end
@ DAEMONS -- IDLE -- the whole state restored at once, then the process goes quiet: a drain, one snap back where RESTORE
@ takes three, and a slow dim that settles. The sleep that follows has its own status animation.
@ T-139, approved 2026-09-17 under vision.md 9.24.
Move_REST:
	monbg ANIM_ATTACKER
	playsewithpan SE_M_TELEPORT, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 13, RGB(13, 13, 13)
	waitforvisualfinish
	delay 8
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 13, 0, RGB(13, 13, 13)
	waitforvisualfinish
	delay 12
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 4, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	delay 20
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	clearmonbg ANIM_ATTACKER
	end

@ genanims: CONTEXT (T-134, vision.md 9.24) approved.
Move_CONFUSION:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 0, 9, RGB(22, 10, 19)
	waitforvisualfinish
	playsewithpan SE_M_SUPERSONIC, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 4, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 5, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 5, 0, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 5, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 5, 0, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 5, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 5, 0, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 9, 0, RGB(22, 10, 19)
	waitforvisualfinish
	end
@ genanims: CONTEXT (T-134, vision.md 9.24) approved.
Move_PSYCHIC:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 0, 9, RGB(22, 10, 19)
	waitforvisualfinish
	playsewithpan SE_M_SUPERSONIC, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 6, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 13, RGB(22, 10, 19)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 13, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 9, 0, RGB(22, 10, 19)
	waitforvisualfinish
	end
@ genanims: CONTEXT (T-134, vision.md 9.24) approved.
Move_FUTURE_SIGHT:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 0, 9, RGB(22, 10, 19)
	waitforvisualfinish
	playsewithpan SE_M_PSYBEAM, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 5, RGB(22, 10, 19)
	waitforvisualfinish
	delay 16
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 5, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 9, 0, RGB(22, 10, 19)
	waitforvisualfinish
	end
@ genanims: SIGNAL (T-134, vision.md 9.24) approved.
Move_THUNDER:
	playsewithpan SE_M_THUNDER_WAVE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 5, 0, 7, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_BATTLERS, 0, 0, 14, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_BATTLERS, 0, 14, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 14, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 14, 0, RGB(10, 23, 23)
	waitforvisualfinish
	end
@ genanims: SIGNAL (T-134, vision.md 9.24) approved.
Move_THUNDER_PUNCH:
	playsewithpan SE_M_COMET_PUNCH, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(13, 13, 13)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 10, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: ENTROPY (T-134, vision.md 9.24) approved.
Move_SACRED_FIRE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(27, 19, 5)
	waitforvisualfinish
	playsewithpan SE_M_SACRED_FIRE, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 9, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 13, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 13, 3, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 10, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 1, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 1, 7, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 7, 4, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 11, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 2, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 2, 6, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 3, 6, 0, RGB(27, 19, 5)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_SCRATCH:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_SCRATCH, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 4, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: EMERGENT (T-134, vision.md 9.24) approved.
Move_DRAGON_BREATH:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 3, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 3, 0, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 3, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 0, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 3, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 3, 0, RGB(6, 15, 13)
	waitforvisualfinish
	playsewithpan SE_M_FLAME_WHEEL, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_DEF_SIDE, 0, 0, 9, RGB(6, 15, 13)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(6, 15, 13)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_DEF_SIDE, 1, 9, 0, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 11, 0, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 6, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 0, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 6, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 0, RGB(6, 15, 13)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_ROAR:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_COMET_PUNCH, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_SlideOffScreen, 5, ANIM_TARGET, 2
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: LOWER (T-134, vision.md 9.24) approved.
Move_GROWL:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_COMET_PUNCH, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 5, RGB(13, 13, 13)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 5, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_SNORE:
	playsewithpan SE_M_SNORE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_DEF_SIDE, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_DEF_SIDE, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_SNORE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_DEF_SIDE, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_DEF_SIDE, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_LIGHT_SCREEN:
	playsewithpan SE_M_REFLECT, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 0, 8, RGB(22, 10, 19)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 8, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 0, 8, RGB(22, 10, 19)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 8, 0, RGB(22, 10, 19)
	waitforvisualfinish
	end
SpecialScreenSparkle:
	createsprite gSpecialScreenSparkleSpriteTemplate, ANIM_ATTACKER, 2, 23, 0, ANIM_ATTACKER, TRUE
	delay 6
	createsprite gSpecialScreenSparkleSpriteTemplate, ANIM_ATTACKER, 2, 31, -8, ANIM_ATTACKER, TRUE
	delay 5
	createsprite gSpecialScreenSparkleSpriteTemplate, ANIM_ATTACKER, 2, 30, 20, ANIM_ATTACKER, TRUE
	delay 7
	createsprite gSpecialScreenSparkleSpriteTemplate, ANIM_ATTACKER, 2, 10, -15, ANIM_ATTACKER, TRUE
	delay 6
	createsprite gSpecialScreenSparkleSpriteTemplate, ANIM_ATTACKER, 2, 20, 10, ANIM_ATTACKER, TRUE
	delay 6
	createsprite gSpecialScreenSparkleSpriteTemplate, ANIM_ATTACKER, 2, 10, 18, ANIM_ATTACKER, TRUE
	return

@ genanims: CONTEXT (T-134, vision.md 9.24) approved.
Move_MIRROR_COAT:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 0, 9, RGB(22, 10, 19)
	waitforvisualfinish
	playsewithpan SE_M_REFLECT, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 3, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 4, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 4, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 16, RGB(22, 10, 19)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 16, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 9, 0, RGB(22, 10, 19)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_REFLECT:
	playsewithpan SE_M_REFLECT, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 0, 8, RGB(22, 10, 19)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 8, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 0, 8, RGB(22, 10, 19)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 8, 0, RGB(22, 10, 19)
	waitforvisualfinish
	end
@ genanims: RAISE (T-134, vision.md 9.24) approved.
Move_BARRIER:
	playsewithpan SE_M_BARRIER, SOUND_PAN_ATTACKER
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(22, 10, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 6, 12, RGB(22, 10, 19)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 12, 0, RGB(22, 10, 19)
	waitforvisualfinish
	end
@ genanims: FLOW (T-134, vision.md 9.24) approved.
Move_BUBBLE:
	playsewithpan SE_M_BUBBLE, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 4, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 0, RGB(8, 13, 22)
	waitforvisualfinish
	playsewithpan SE_M_BUBBLE, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 4, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 0, RGB(8, 13, 22)
	waitforvisualfinish
	playsewithpan SE_M_BUBBLE, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 4, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 0, RGB(8, 13, 22)
	waitforvisualfinish
	end
@ genanims: CORRUPT (T-134, vision.md 9.24) approved.
Move_SMOG:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 0, 6, RGB(10, 11, 6)
	waitforvisualfinish
	playsewithpan SE_M_MIST, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 8, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 2, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 2, 6, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 1, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 1, 4, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 1, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 1, 0, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 3, 0, 3, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 3, 3, 0, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 2, 6, 0, RGB(10, 11, 6)
	waitforvisualfinish
	end
@ genanims: OPAQUE (T-134, vision.md 9.24) approved.
Move_FAINT_ATTACK:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 14, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 15, RGB(5, 5, 5)
	waitforvisualfinish
	playsewithpan SE_M_FAINT_ATTACK, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 1
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 15, 0, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 14, 0, RGB(5, 5, 5)
	waitforvisualfinish
	end
@ genanims: LOWER (T-134, vision.md 9.24) approved.
Move_SAND_ATTACK:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(19, 15, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(19, 15, 9)
	waitforvisualfinish
	playsewithpan SE_M_SAND_ATTACK, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(19, 15, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(19, 15, 9)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET | F_PAL_BG, 1, 0, 5, RGB(13, 13, 13)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET | F_PAL_BG, 1, 5, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: STRATUM (T-134, vision.md 9.24) approved.
Move_MUD_SLAP:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 5, RGB(19, 15, 9)
	waitforvisualfinish
	playsewithpan SE_M_SAND_ATTACK, SOUND_PAN_TARGET
	createvisualtask AnimTask_HorizontalShake, 5, (MAX_BATTLERS_COUNT + 1), 2, 8
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 2, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(19, 15, 9)
	waitforvisualfinish
	delay 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 8, 0, RGB(19, 15, 9)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 5, 0, RGB(19, 15, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 7, RGB(13, 13, 13)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 7, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: EMERGENT (T-134, vision.md 9.24) approved.
Move_DRAGON_RAGE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 3, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 3, 0, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 3, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 0, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 3, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 3, 0, RGB(6, 15, 13)
	waitforvisualfinish
	playsewithpan SE_M_DRAGON_RAGE, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_DEF_SIDE, 0, 0, 9, RGB(6, 15, 13)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(6, 15, 13)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_DEF_SIDE, 1, 9, 0, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 10, 0, RGB(6, 15, 13)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_RAIN_DANCE:
	playsewithpan SE_M_RAIN_DANCE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 0, 9, RGB(8, 13, 22)
	waitforvisualfinish
	delay 12
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 9, 0, RGB(8, 13, 22)
	waitforvisualfinish
	end
@ genanims: OPAQUE (T-134, vision.md 9.24) approved.
Move_BITE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 15, RGB(5, 5, 5)
	waitforvisualfinish
	playsewithpan SE_M_BITE, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 1
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 15, 0, RGB(5, 5, 5)
	waitforvisualfinish
	end
@ genanims: OPAQUE (T-134, vision.md 9.24) approved.
Move_CRUNCH:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 15, RGB(5, 5, 5)
	waitforvisualfinish
	playsewithpan SE_M_BITE, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 5, 1
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 15, 0, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 7, RGB(13, 13, 13)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 7, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: FLOW (T-134, vision.md 9.24) approved.
Move_CLAMP:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(8, 13, 22)
	waitforvisualfinish
	playsewithpan SE_M_VICEGRIP, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 9, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 1, 2, 2
	delay 8
	playsewithpan SE_M_VICEGRIP, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 1, 2, 2
	delay 8
	playsewithpan SE_M_VICEGRIP, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 1, 2, 2
	delay 8
	playsewithpan SE_M_VICEGRIP, SOUND_PAN_TARGET
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 9, 0, RGB(8, 13, 22)
	waitforvisualfinish
	end
@ genanims: FROZEN (T-134, vision.md 9.24) approved.
Move_ICE_BEAM:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(20, 25, 27)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(20, 25, 27)
	waitforvisualfinish
	playsewithpan SE_M_BUBBLE_BEAM2, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 13, RGB(20, 25, 27)
	waitforvisualfinish
	delay 24
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 13, 0, RGB(20, 25, 27)
	waitforvisualfinish
	end
@ genanims: RAISE (T-134, vision.md 9.24) approved.
Move_WITHDRAW:
	playsewithpan SE_M_HEADBUTT, SOUND_PAN_ATTACKER
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(8, 13, 22)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 6, 0, RGB(8, 13, 22)
	waitforvisualfinish
	end
@ genanims: FROZEN (T-134, vision.md 9.24) approved.
Move_AURORA_BEAM:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(20, 25, 27)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(20, 25, 27)
	waitforvisualfinish
	playsewithpan SE_M_BUBBLE_BEAM, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(20, 25, 27)
	waitforvisualfinish
	delay 24
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 0, RGB(20, 25, 27)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 6, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 6, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: GROWTH (T-134, vision.md 9.24) approved.
Move_SOLAR_BEAM:
	choosetwoturnanim DaemonsBatchGather, DaemonsBatchFit
DaemonsBatchDone:
	end
DaemonsBatchGather:
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 0, 4, RGB(11, 19, 12)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 4, 8, RGB(11, 19, 12)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 8, 12, RGB(11, 19, 12)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 12, 0, RGB(11, 19, 12)
	waitforvisualfinish
	goto DaemonsBatchDone
DaemonsBatchFit:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 6, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(11, 19, 12)
	waitforvisualfinish
	playsewithpan SE_M_MEGA_KICK, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 5, 0, 6, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 14, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 14, 4, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 9, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 7, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 7, 0, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 6, 0, RGB(11, 19, 12)
	waitforvisualfinish
	goto DaemonsBatchDone
	end
@ genanims: FROZEN (T-134, vision.md 9.24) approved.
Move_BLIZZARD:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(20, 25, 27)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(20, 25, 27)
	waitforvisualfinish
	playsewithpan SE_M_BLIZZARD, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 7, RGB(20, 25, 27)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 0, 14, RGB(20, 25, 27)
	waitforvisualfinish
	delay 24
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 14, 0, RGB(20, 25, 27)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 7, 0, RGB(20, 25, 27)
	waitforvisualfinish
	end
@ genanims: FROZEN (T-134, vision.md 9.24) approved.
Move_POWDER_SNOW:
	playsewithpan SE_M_GUST, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(20, 25, 27)
	waitforvisualfinish
	delay 14
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(20, 25, 27)
	waitforvisualfinish
	end
@ genanims: FLOW (T-134, vision.md 9.24) approved.
Move_HYDRO_PUMP:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 0, 6, RGB(8, 13, 22)
	waitforvisualfinish
	playsewithpan SE_M_HYDRO_PUMP, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 5, 7, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 14, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 3, 14, 0, RGB(8, 13, 22)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 3, 6, 0, RGB(8, 13, 22)
	waitforvisualfinish
	end
@ genanims: SWARM (T-134, vision.md 9.24) approved.
Move_SIGNAL_BEAM:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(16, 18, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_BUBBLE_BEAM, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 2, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_BUBBLE_BEAM, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 2, 4, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_BUBBLE_BEAM, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 6, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_BUBBLE_BEAM, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 8, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_BUBBLE_BEAM, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 10, RGB(16, 18, 9)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(16, 18, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 7, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 7, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: GROWTH (T-134, vision.md 9.24) approved.
Move_ABSORB:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(11, 19, 12)
	waitforvisualfinish
	playsewithpan SE_M_ABSORB, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 8, 0, RGB(11, 19, 12)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 4, RGB(11, 19, 12)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 4, 0, RGB(11, 19, 12)
	waitforvisualfinish
	end
AbsorbEffect:
	playsewithpan SE_M_CRABHAMMER, SOUND_PAN_TARGET
	createsprite gAbsorptionOrbSpriteTemplate, ANIM_ATTACKER, 3, 0, 5, 8, 26
	delay 4
	playsewithpan SE_M_CRABHAMMER, SOUND_PAN_TARGET
	createsprite gAbsorptionOrbSpriteTemplate, ANIM_ATTACKER, 3, 10, -5, -8, 26
	delay 4
	playsewithpan SE_M_CRABHAMMER, SOUND_PAN_TARGET
	createsprite gAbsorptionOrbSpriteTemplate, ANIM_ATTACKER, 3, -5, 15, 16, 33
	delay 4
	playsewithpan SE_M_CRABHAMMER, SOUND_PAN_TARGET
	createsprite gAbsorptionOrbSpriteTemplate, ANIM_ATTACKER, 3, 0, -15, -16, 36
	delay 4
	playsewithpan SE_M_CRABHAMMER, SOUND_PAN_TARGET
	createsprite gAbsorptionOrbSpriteTemplate, ANIM_ATTACKER, 3, 0, 5, 8, 26
	delay 4
	playsewithpan SE_M_CRABHAMMER, SOUND_PAN_TARGET
	createsprite gAbsorptionOrbSpriteTemplate, ANIM_ATTACKER, 3, 10, -5, -8, 26
	delay 4
	playsewithpan SE_M_CRABHAMMER, SOUND_PAN_TARGET
	createsprite gAbsorptionOrbSpriteTemplate, ANIM_ATTACKER, 3, -10, 20, 20, 39
	delay 4
	playsewithpan SE_M_CRABHAMMER, SOUND_PAN_TARGET
	createsprite gAbsorptionOrbSpriteTemplate, ANIM_ATTACKER, 3, 5, -18, -20, 35
	delay 4
	return

@ genanims: GROWTH (T-134, vision.md 9.24) approved.
Move_MEGA_DRAIN:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(11, 19, 12)
	waitforvisualfinish
	playsewithpan SE_M_ABSORB, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 9, 0, RGB(11, 19, 12)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 4, RGB(11, 19, 12)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 4, 8, RGB(11, 19, 12)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 8, 0, RGB(11, 19, 12)
	waitforvisualfinish
	end
@ genanims: GROWTH (T-134, vision.md 9.24) approved.
Move_GIGA_DRAIN:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(11, 19, 12)
	waitforvisualfinish
	playsewithpan SE_M_ABSORB, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 11, 0, RGB(11, 19, 12)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 4, RGB(11, 19, 12)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 4, 8, RGB(11, 19, 12)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 8, 12, RGB(11, 19, 12)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 12, 0, RGB(11, 19, 12)
	waitforvisualfinish
	end
@ genanims: SWARM (T-134, vision.md 9.24) approved.
Move_LEECH_LIFE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(16, 18, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_ABSORB, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 2, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_ABSORB, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 2, 4, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_ABSORB, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 6, RGB(16, 18, 9)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 0, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_ABSORB, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(16, 18, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 8, 0, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 4, RGB(16, 18, 9)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 4, 0, RGB(16, 18, 9)
	waitforvisualfinish
	end
@ DAEMONS -- REGROW -- grown back, not reloaded: a dip toward grey, and what returns comes back through GROWTH, the
@ routine's own type, before settling into the daemon's colour.
@ T-139, approved 2026-09-17 under vision.md 9.24.
Move_SYNTHESIS:
	monbg ANIM_ATTACKER
	playsewithpan SE_M_TELEPORT, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 9, RGB(13, 13, 13)
	waitforvisualfinish
	delay 6
	playsewithpan SE_M_MEGA_KICK, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 11, RGB(11, 19, 12)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 3, 11, 0, RGB(11, 19, 12)
	waitforvisualfinish
	clearmonbg ANIM_ATTACKER
	end

@ genanims: AFFLICT (T-134, vision.md 9.24) approved.
Move_TOXIC:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(10, 11, 6)
	waitforvisualfinish
	playsewithpan SE_M_TOXIC, SOUND_PAN_TARGET
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 0, 4, RGB(10, 11, 6)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 4, 8, RGB(10, 11, 6)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 8, 12, RGB(10, 11, 6)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 12, 16, RGB(10, 11, 6)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 16, 0, RGB(10, 11, 6)
	waitforvisualfinish
	end
@ genanims: CORRUPT (T-134, vision.md 9.24) approved.
Move_SLUDGE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(10, 11, 6)
	waitforvisualfinish
	playsewithpan SE_M_BUBBLE3, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 11, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 2, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 2, 8, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 1, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 1, 5, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 5, 1, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 1, 0, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 3, 0, 3, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 3, 3, 0, RGB(10, 11, 6)
	waitforvisualfinish
	end
@ genanims: CORRUPT (T-134, vision.md 9.24) approved.
Move_SLUDGE_BOMB:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 0, 6, RGB(10, 11, 6)
	waitforvisualfinish
	playsewithpan SE_M_DIG, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 6, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 13, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 13, 3, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 9, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 1, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 1, 6, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 2, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 2, 0, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 3, 0, 3, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 3, 3, 0, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 2, 6, 0, RGB(10, 11, 6)
	waitforvisualfinish
	end
@ genanims: CORRUPT (T-134, vision.md 9.24) approved.
Move_ACID:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(10, 11, 6)
	waitforvisualfinish
	playsewithpan SE_M_BUBBLE3, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 4, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 1, 0, 9, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 9, 2, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 2, 6, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 6, 1, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 1, 4, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 4, 1, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 1, 0, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 3, 0, 3, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 3, 3, 0, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 7, RGB(13, 13, 13)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 7, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: STRATUM (T-134, vision.md 9.24) approved.
Move_BONEMERANG:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 5, RGB(19, 15, 9)
	waitforvisualfinish
	playsewithpan SE_M_BONEMERANG, SOUND_PAN_TARGET
	createvisualtask AnimTask_HorizontalShake, 5, (MAX_BATTLERS_COUNT + 1), 2, 8
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 5, 4, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(19, 15, 9)
	waitforvisualfinish
	delay 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 10, 0, RGB(19, 15, 9)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 5, 0, RGB(19, 15, 9)
	waitforvisualfinish
	end
@ genanims: STRATUM (T-134, vision.md 9.24) approved.
Move_BONE_CLUB:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(19, 15, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(19, 15, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 8, RGB(19, 15, 9)
	waitforvisualfinish
	playsewithpan SE_M_BONEMERANG, SOUND_PAN_TARGET
	createvisualtask AnimTask_HorizontalShake, 5, (MAX_BATTLERS_COUNT + 1), 2, 8
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 5, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(19, 15, 9)
	waitforvisualfinish
	delay 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 11, 0, RGB(19, 15, 9)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 8, 0, RGB(19, 15, 9)
	waitforvisualfinish
	end
@ genanims: STRATUM (T-134, vision.md 9.24) approved.
Move_BONE_RUSH:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 5, RGB(19, 15, 9)
	waitforvisualfinish
	playsewithpan SE_M_BONEMERANG, SOUND_PAN_TARGET
	createvisualtask AnimTask_HorizontalShake, 5, (MAX_BATTLERS_COUNT + 1), 2, 8
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 4, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(19, 15, 9)
	waitforvisualfinish
	delay 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 8, 0, RGB(19, 15, 9)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 5, 0, RGB(19, 15, 9)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_SPIKES:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(19, 15, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(19, 15, 9)
	waitforvisualfinish
	playsewithpan SE_M_JUMP_KICK, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 7, RGB(19, 15, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 7, 0, RGB(19, 15, 9)
	waitforvisualfinish
	delay 4
	playsewithpan SE_M_JUMP_KICK, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 7, RGB(19, 15, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 7, 0, RGB(19, 15, 9)
	waitforvisualfinish
	delay 4
	playsewithpan SE_M_JUMP_KICK, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 7, RGB(19, 15, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 7, 0, RGB(19, 15, 9)
	waitforvisualfinish
	delay 4
	end
@ genanims: SWARM (T-134, vision.md 9.24) approved.
Move_MEGAHORN:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(16, 18, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_DRAGON_RAGE, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 2, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_DRAGON_RAGE, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 2, 4, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_DRAGON_RAGE, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 6, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_DRAGON_RAGE, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 8, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_DRAGON_RAGE, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 10, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_DRAGON_RAGE, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 12, RGB(16, 18, 9)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 0, RGB(16, 18, 9)
	waitforvisualfinish
	end
@ genanims: VECTOR (T-134, vision.md 9.24) approved.
Move_GUST:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 8, 0, RGB(25, 8, 8)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 0, 5, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 5, 0, RGB(25, 8, 8)
	playsewithpan SE_M_GUST, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 6, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 9, 0, RGB(25, 8, 8)
	waitforvisualfinish
	end
@ genanims: VECTOR (T-134, vision.md 9.24) approved.
Move_WING_ATTACK:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 8, 0, RGB(25, 8, 8)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 5, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 5, 0, RGB(25, 8, 8)
	playsewithpan SE_M_WING_ATTACK, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 0, RGB(25, 8, 8)
	waitforvisualfinish
	end
@ genanims: VECTOR (T-134, vision.md 9.24) approved.
Move_PECK:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 5, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 5, 0, RGB(25, 8, 8)
	playsewithpan SE_M_HORN_ATTACK, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 4, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(25, 8, 8)
	waitforvisualfinish
	end
@ genanims: VECTOR (T-134, vision.md 9.24) approved.
Move_AEROBLAST:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 8, 0, RGB(25, 8, 8)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 5, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 5, 0, RGB(25, 8, 8)
	playsewithpan SE_M_RAZOR_WIND, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 6, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 13, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 13, 0, RGB(25, 8, 8)
	waitforvisualfinish
	end
@ genanims: FLOW (T-134, vision.md 9.24) approved.
Move_WATER_GUN:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 6, RGB(8, 13, 22)
	waitforvisualfinish
	playsewithpan SE_M_BUBBLE, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 3, 4, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 9, 0, RGB(8, 13, 22)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 2, 6, 0, RGB(8, 13, 22)
	waitforvisualfinish
	end
@ genanims: FLOW (T-134, vision.md 9.24) approved.
Move_CRABHAMMER:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 0, 6, RGB(8, 13, 22)
	waitforvisualfinish
	playsewithpan SE_M_VITAL_THROW2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 4, 6, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 13, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 13, 0, RGB(8, 13, 22)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 6, 0, RGB(8, 13, 22)
	waitforvisualfinish
	end
@ genanims: FLOW (T-134, vision.md 9.24) approved.
Move_SURF:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 0, 6, RGB(8, 13, 22)
	waitforvisualfinish
	playsewithpan SE_M_SURF, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 4, 6, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 0, 13, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 2, 13, 0, RGB(8, 13, 22)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 2, 6, 0, RGB(8, 13, 22)
	waitforvisualfinish
	end
@ genanims: ENTROPY (T-134, vision.md 9.24) approved.
Move_FLAMETHROWER:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(27, 19, 5)
	waitforvisualfinish
	playsewithpan SE_M_FLAMETHROWER, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 9, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 13, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 13, 3, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 10, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 1, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 1, 7, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 7, 4, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 11, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 2, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 2, 6, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 6, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 6, 2, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 2, 5, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 5, 1, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 1, 4, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 4, 0, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 6, 0, RGB(27, 19, 5)
	waitforvisualfinish
	end
@ Also used by Sandstorm weather
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_SANDSTORM:
	playsewithpan SE_M_SANDSTORM, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 0, 9, RGB(16, 16, 17)
	waitforvisualfinish
	delay 12
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 9, 0, RGB(16, 16, 17)
	waitforvisualfinish
	end
@ genanims: FLOW (T-134, vision.md 9.24) approved.
Move_WHIRLPOOL:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(8, 13, 22)
	waitforvisualfinish
	playsewithpan SE_M_WHIRLPOOL, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 8, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 1, 2, 2
	delay 8
	playsewithpan SE_M_WHIRLPOOL, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 1, 2, 2
	delay 8
	playsewithpan SE_M_WHIRLPOOL, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 1, 2, 2
	delay 8
	playsewithpan SE_M_WHIRLPOOL, SOUND_PAN_TARGET
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 8, 0, RGB(8, 13, 22)
	waitforvisualfinish
	end
WhirlpoolEffect:
	createsprite gWhirlpoolSpriteTemplate, ANIM_TARGET, 2, 0, 28, 384, 50, 8, 50, ANIM_TARGET
	delay 2
	createsprite gWhirlpoolSpriteTemplate, ANIM_TARGET, 2, 0, 32, 240, 40, 11, -46, ANIM_TARGET
	delay 2
	createsprite gWhirlpoolSpriteTemplate, ANIM_TARGET, 2, 0, 33, 416, 40, 4, 42, ANIM_TARGET
	delay 2
	createsprite gWhirlpoolSpriteTemplate, ANIM_TARGET, 2, 0, 31, 288, 45, 6, -42, ANIM_TARGET
	delay 2
	createsprite gWhirlpoolSpriteTemplate, ANIM_TARGET, 2, 0, 28, 448, 45, 11, 46, ANIM_TARGET
	delay 2
	createsprite gWhirlpoolSpriteTemplate, ANIM_TARGET, 2, 0, 33, 464, 50, 10, -50, ANIM_TARGET
	delay 2
	return

@ genanims: VECTOR (T-134, vision.md 9.24) approved.
Move_FLY:
	choosetwoturnanim DaemonsGotoAway, DaemonsGotoBack
DaemonsGotoDone:
	end
DaemonsGotoAway:
	playsewithpan SE_M_FLY, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 12, RGB(13, 13, 13)
	waitforvisualfinish
	invisible ANIM_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 12, 0, RGB(13, 13, 13)
	waitforvisualfinish
	goto DaemonsGotoDone
DaemonsGotoBack:
	visible ANIM_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 8, 0, RGB(25, 8, 8)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 5, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 5, 0, RGB(25, 8, 8)
	playsewithpan SE_M_FLY, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 0, RGB(25, 8, 8)
	waitforvisualfinish
	goto DaemonsGotoDone
	end
@ genanims: VECTOR (T-134, vision.md 9.24) approved.
Move_BOUNCE:
	choosetwoturnanim DaemonsReboundAway, DaemonsReboundBack
DaemonsReboundDone:
	end
DaemonsReboundAway:
	playsewithpan SE_M_TELEPORT, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 12, RGB(13, 13, 13)
	waitforvisualfinish
	invisible ANIM_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 12, 0, RGB(13, 13, 13)
	waitforvisualfinish
	goto DaemonsReboundDone
DaemonsReboundBack:
	visible ANIM_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 8, 0, RGB(25, 8, 8)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 5, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 5, 0, RGB(25, 8, 8)
	playsewithpan SE_M_TELEPORT, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 0, RGB(25, 8, 8)
	waitforvisualfinish
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 4, 4, 1
	waitforvisualfinish
	goto DaemonsReboundDone
	end
@ genanims: LOGIC (T-134, vision.md 9.24) approved.
Move_KARATE_CHOP:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(12, 15, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 3, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 6, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 9, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_DOUBLE_TEAM, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 4, 1
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(12, 15, 19)
	waitforvisualfinish
	end
@ genanims: LOGIC (T-134, vision.md 9.24) approved.
Move_CROSS_CHOP:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(12, 15, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 3, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 6, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 9, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 12, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MEGA_KICK, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 6, 1
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 0, RGB(12, 15, 19)
	waitforvisualfinish
	end
@ genanims: LOGIC (T-134, vision.md 9.24) approved.
Move_JUMP_KICK:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(12, 15, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 3, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 6, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 9, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_JUMP_KICK, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 1
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(12, 15, 19)
	waitforvisualfinish
	end
@ genanims: LOGIC (T-134, vision.md 9.24) approved.
Move_HI_JUMP_KICK:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(12, 15, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 4, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 8, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 12, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_JUMP_KICK, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 5, 1
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 0, RGB(12, 15, 19)
	waitforvisualfinish
	end
@ genanims: LOGIC (T-134, vision.md 9.24) approved.
Move_DOUBLE_KICK:
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 4, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 8, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_VITAL_THROW2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 4, 1
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(12, 15, 19)
	waitforvisualfinish
	end
@ genanims: LOGIC (T-134, vision.md 9.24) approved.
Move_TRIPLE_KICK:
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 4, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 8, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_VITAL_THROW2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(12, 15, 19)
	waitforvisualfinish
	end
@ genanims: LOGIC (T-134, vision.md 9.24) approved.
Move_DYNAMIC_PUNCH:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(12, 15, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 3, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 12, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 6, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_VITAL_THROW2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 6, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 0, RGB(12, 15, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: LOGIC (T-134, vision.md 9.24) approved.
Move_COUNTER:
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 5, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 5, 10, RGB(12, 15, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 10, 0, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 4, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 8, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_VITAL_THROW, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(12, 15, 19)
	waitforvisualfinish
	end
@ genanims: LOGIC (T-134, vision.md 9.24) approved.
Move_VITAL_THROW:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(12, 15, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 3, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 6, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 9, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_VITAL_THROW, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 1
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(12, 15, 19)
	waitforvisualfinish
	end
@ genanims: LOGIC (T-134, vision.md 9.24) approved.
Move_ROCK_SMASH:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(12, 15, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 4, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 8, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_VITAL_THROW2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(12, 15, 19)
	waitforvisualfinish
	end
@ genanims: LOGIC (T-134, vision.md 9.24) approved.
Move_SUBMISSION:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(12, 15, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 4, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 8, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 12, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_DOUBLE_TEAM, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 5, 1
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 0, RGB(12, 15, 19)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ Also used by Sunny weather
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_SUNNY_DAY:
	playsewithpan SE_M_PETAL_DANCE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 0, 9, RGB(27, 19, 5)
	waitforvisualfinish
	delay 12
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 9, 0, RGB(27, 19, 5)
	waitforvisualfinish
	end
@ genanims: LOWER (T-134, vision.md 9.24) approved.
Move_COTTON_SPORE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(11, 19, 12)
	waitforvisualfinish
	playsewithpan SE_M_POISON_POWDER, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(11, 19, 12)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 5, RGB(13, 13, 13)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 5, 10, RGB(13, 13, 13)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 3, 10, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: AFFLICT (T-134, vision.md 9.24) approved.
Move_SPORE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(11, 19, 12)
	waitforvisualfinish
	playsewithpan SE_M_POISON_POWDER, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 6, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 0, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 3, 0, 12, RGB(13, 13, 13)
	waitforvisualfinish
	delay 16
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 12, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: GROWTH (T-134, vision.md 9.24) approved.
Move_PETAL_DANCE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(11, 19, 12)
	waitforvisualfinish
	playsewithpan SE_M_PETAL_DANCE, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 4, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 3, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 1, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 1, 7, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 7, 3, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 11, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 11, 0, RGB(11, 19, 12)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: GROWTH (T-134, vision.md 9.24) approved.
Move_RAZOR_LEAF:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(11, 19, 12)
	waitforvisualfinish
	playsewithpan SE_M_POISON_POWDER, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 0, 10, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 10, 3, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 3, 6, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 6, 5, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 1, 5, 0, RGB(11, 19, 12)
	waitforvisualfinish
	end
@ genanims: COPY (T-134, vision.md 9.24) approved.
Move_NATURE_POWER:
	playsewithpan SE_M_COMET_PUNCH, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: LEGACY (T-134, vision.md 9.24) approved.
Move_ANCIENT_POWER:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 0, 7, RGB(26, 26, 26)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 7, 0, RGB(26, 26, 26)
	waitforvisualfinish
	delay 6
	playsewithpan SE_M_DRAGON_RAGE, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 7, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 7, 3, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 0, RGB(26, 26, 26)
	waitforvisualfinish
	playsewithpan SE_M_DRAGON_RAGE, SOUND_PAN_ATTACKER
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(26, 26, 26)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 6, 0, RGB(26, 26, 26)
	waitforvisualfinish
	end
@ genanims: FLOW (T-134, vision.md 9.24) approved.
Move_OCTAZOOKA:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 0, 6, RGB(8, 13, 22)
	waitforvisualfinish
	playsewithpan SE_M_MEGA_KICK2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 3, 5, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 11, 0, RGB(8, 13, 22)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 2, 6, 0, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 7, RGB(13, 13, 13)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 7, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_MIST:
	playsewithpan SE_M_MIST, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 0, 8, RGB(20, 25, 27)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 8, 0, RGB(20, 25, 27)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 0, 8, RGB(20, 25, 27)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 8, 0, RGB(20, 25, 27)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_HAZE:
	playsewithpan SE_M_HAZE, SOUND_PAN_ATTACKER
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_BATTLERS, 1, 0, 12, RGB(13, 13, 13)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_BATTLERS, 1, 12, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: ENTROPY (T-134, vision.md 9.24) approved.
Move_FIRE_PUNCH:
	playsewithpan SE_M_FLAME_WHEEL, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 8, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 3, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 9, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 1, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 1, 7, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 3, 7, 0, RGB(27, 19, 5)
	waitforvisualfinish
	end
FireSpreadEffect:
	createsprite gFireSpreadSpriteTemplate, ANIM_TARGET, 1, 0, 10, 192, 176, 40
	createsprite gFireSpreadSpriteTemplate, ANIM_TARGET, 1, 0, 10, -192, 240, 40
	createsprite gFireSpreadSpriteTemplate, ANIM_TARGET, 1, 0, 10, 192, -160, 40
	createsprite gFireSpreadSpriteTemplate, ANIM_TARGET, 1, 0, 10, -192, -112, 40
	createsprite gFireSpreadSpriteTemplate, ANIM_TARGET, 1, 0, 10, 160, 48, 40
	createsprite gFireSpreadSpriteTemplate, ANIM_TARGET, 1, 0, 10, -224, -32, 40
	createsprite gFireSpreadSpriteTemplate, ANIM_TARGET, 1, 0, 10, 112, -128, 40
	return

@ genanims: LOWER (T-134, vision.md 9.24) approved.
Move_LEER:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_LEER, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 5, RGB(13, 13, 13)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 5, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: CONTEXT (T-134, vision.md 9.24) approved.
Move_DREAM_EATER:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 0, 9, RGB(22, 10, 19)
	waitforvisualfinish
	playsewithpan SE_M_PSYBEAM, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 6, 2
	playsewithpan SE_M_PSYBEAM, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 13, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 13, 0, RGB(22, 10, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 4, RGB(22, 10, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 4, 8, RGB(22, 10, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 8, 12, RGB(22, 10, 19)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 12, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 9, 0, RGB(22, 10, 19)
	waitforvisualfinish
	end
@ genanims: AFFLICT (T-134, vision.md 9.24) approved.
Move_POISON_GAS:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(10, 11, 6)
	waitforvisualfinish
	playsewithpan SE_M_MIST, SOUND_PAN_TARGET
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 0, 4, RGB(10, 11, 6)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 4, 8, RGB(10, 11, 6)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 8, 12, RGB(10, 11, 6)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 12, 0, RGB(10, 11, 6)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_BIND:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_BIND, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 1, 2, 2
	delay 8
	playsewithpan SE_M_BIND, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 1, 2, 2
	delay 8
	playsewithpan SE_M_BIND, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 1, 2, 2
	delay 8
	playsewithpan SE_M_BIND, SOUND_PAN_TARGET
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
BindWrap:
	playsewithpan SE_M_BIND, SOUND_PAN_TARGET
	call BindWrapSqueezeTarget
	call BindWrapSqueezeTarget
	waitforvisualfinish
	end

BindWrapSqueezeTarget:
	createvisualtask AnimTask_ScaleMonAndRestore, 5, 10, -5, 5, ANIM_TARGET, 0
	delay 16
	return

@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_WRAP:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_COMET_PUNCH, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 1, 2, 2
	delay 8
	playsewithpan SE_M_COMET_PUNCH, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 1, 2, 2
	delay 8
	playsewithpan SE_M_COMET_PUNCH, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 1, 2, 2
	delay 8
	playsewithpan SE_M_COMET_PUNCH, SOUND_PAN_TARGET
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: CONTEXT (T-134, vision.md 9.24) approved.
Move_PSYBEAM:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 0, 9, RGB(22, 10, 19)
	waitforvisualfinish
	playsewithpan SE_M_PSYBEAM, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 5, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 11, RGB(22, 10, 19)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 11, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 9, 0, RGB(22, 10, 19)
	waitforvisualfinish
	end
@ genanims: AFFLICT (T-134, vision.md 9.24) approved.
Move_HYPNOSIS:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(22, 10, 19)
	waitforvisualfinish
	playsewithpan SE_M_SUPERSONIC, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 6, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 3, 0, 12, RGB(13, 13, 13)
	waitforvisualfinish
	delay 16
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 12, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: CONTEXT (T-134, vision.md 9.24) approved.
Move_PSYWAVE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 0, 9, RGB(22, 10, 19)
	waitforvisualfinish
	playsewithpan SE_M_PSYBEAM, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 5, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 5, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 5, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 13, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 13, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 8, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 8, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 9, 0, RGB(22, 10, 19)
	waitforvisualfinish
	end
@ genanims: SIGNAL (T-134, vision.md 9.24) approved.
Move_ZAP_CANNON:
	playsewithpan SE_M_THUNDER_WAVE, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 6, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 4, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 13, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 13, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 16, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 16, 0, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 6, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 0, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 6, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 0, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 6, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: HARDENED (T-134, vision.md 9.24) approved.
Move_STEEL_WING:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 9, 9, RGB(24, 21, 12)
	waitforvisualfinish
	delay 4
	playsewithpan SE_M_HARDEN, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 1, 3
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 11, RGB(24, 21, 12)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 0, RGB(24, 21, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 0, RGB(24, 21, 12)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 11, 11, RGB(24, 21, 12)
	waitforvisualfinish
	delay 18
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 0, RGB(24, 21, 12)
	waitforvisualfinish
	end
@ genanims: HARDENED (T-134, vision.md 9.24) approved.
Move_IRON_TAIL:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 9, 9, RGB(24, 21, 12)
	waitforvisualfinish
	delay 4
	playsewithpan SE_M_HARDEN, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 1, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 7, 7, RGB(24, 21, 12)
	waitforvisualfinish
	delay 3
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 0, RGB(24, 21, 12)
	waitforvisualfinish
	delay 3
	playsewithpan SE_M_HARDEN, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 1, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 7, 7, RGB(24, 21, 12)
	waitforvisualfinish
	delay 3
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 0, RGB(24, 21, 12)
	waitforvisualfinish
	delay 3
	playsewithpan SE_M_HARDEN, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 1, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 7, 7, RGB(24, 21, 12)
	waitforvisualfinish
	delay 3
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 0, RGB(24, 21, 12)
	waitforvisualfinish
	delay 3
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 0, RGB(24, 21, 12)
	waitforvisualfinish
	playsewithpan SE_M_HARDEN, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 1, 3
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 9, RGB(13, 13, 13)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 9, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: CORRUPT (T-134, vision.md 9.24) approved.
Move_POISON_TAIL:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(10, 11, 6)
	waitforvisualfinish
	playsewithpan SE_M_HARDEN, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 4, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 10, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 2, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 2, 7, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 7, 1, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 1, 5, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 5, 1, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 1, 0, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 3, 0, 3, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 3, 3, 0, RGB(10, 11, 6)
	waitforvisualfinish
	end
@ genanims: HARDENED (T-134, vision.md 9.24) approved.
Move_METAL_CLAW:
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 4, 4, RGB(24, 21, 12)
	waitforvisualfinish
	delay 3
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 0, RGB(24, 21, 12)
	waitforvisualfinish
	delay 3
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 7, 7, RGB(24, 21, 12)
	waitforvisualfinish
	delay 3
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 0, RGB(24, 21, 12)
	waitforvisualfinish
	delay 3
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 10, 10, RGB(24, 21, 12)
	waitforvisualfinish
	delay 3
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 0, RGB(24, 21, 12)
	waitforvisualfinish
	delay 3
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 9, 9, RGB(24, 21, 12)
	waitforvisualfinish
	delay 4
	playsewithpan SE_M_HARDEN, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 1, 3
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 10, RGB(24, 21, 12)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 0, RGB(24, 21, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 0, RGB(24, 21, 12)
	waitforvisualfinish
	end
@ genanims: LATENT (T-134, vision.md 9.24) approved.
Move_NIGHT_SHADE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 3, 0, 4, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 3, 4, 0, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 4, 0, 3, RGB(11, 8, 14)
	waitforvisualfinish
	playsewithpan SE_M_PSYBEAM, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 11, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 0, RGB(11, 8, 14)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_EGG_BOMB:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_TAIL_WHIP, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 6, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 13, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 13, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: LATENT (T-134, vision.md 9.24) approved.
Move_SHADOW_BALL:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 3, 0, 4, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 3, 4, 0, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 4, 0, 3, RGB(11, 8, 14)
	waitforvisualfinish
	playsewithpan SE_M_MIST, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 12, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 0, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 7, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 7, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: LATENT (T-134, vision.md 9.24) approved.
Move_LICK:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 0, 3, RGB(11, 8, 14)
	waitforvisualfinish
	playsewithpan SE_M_LICK, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 8, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 6, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 0, RGB(11, 8, 14)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_FOCUS_ENERGY:
	playsewithpan SE_M_DRAGON_RAGE, SOUND_PAN_ATTACKER
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_BIDE:
	choosetwoturnanim DaemonsBideLoad, DaemonsBideRun
DaemonsBideDone:
	end
DaemonsBideLoad:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	delay 12
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	goto DaemonsBideDone
DaemonsBideRun:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_TAKE_DOWN, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	goto DaemonsBideDone
	end
@ genanims: LOWER (T-134, vision.md 9.24) approved.
Move_STRING_SHOT:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(16, 18, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_STRING_SHOT, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(16, 18, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 5, RGB(13, 13, 13)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 3, 5, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_SPIDER_WEB:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(16, 18, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_STRING_SHOT, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(16, 18, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(13, 13, 13)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 12, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_RAZOR_WIND:
	choosetwoturnanim DaemonsRazorWindLoad, DaemonsRazorWindRun
DaemonsRazorWindDone:
	end
DaemonsRazorWindLoad:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	delay 12
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	goto DaemonsRazorWindDone
DaemonsRazorWindRun:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_GUST, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 0, RGB(24, 23, 21)
	waitforvisualfinish
	goto DaemonsRazorWindDone
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_DISABLE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_DETECT, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(13, 13, 13)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 12, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ DAEMONS -- RESTORE (vision.md 9.24, approved 2026-09-17 as the rule's exemplar). Vanilla drew orbs pulled in from outside and a yellow
@ flash: a creature gathering energy. RESTORE puts back what a PROCESS lost, from a state it already had. So what
@ was lost is shown first -- its colour drains to grey, the ground this game stands on (8.6), streaks and all,
@ because the routines it knows are part of what is put back -- and then the state comes back in three steps, a
@ checkpoint reloading, each with a tick. Nothing arrives from outside the daemon. The same tool PERSPECTIVE drains
@ with, so the two routines that are about a daemon's own frame speak one language.
Move_RECOVER:
	monbg ANIM_ATTACKER
	playsewithpan SE_M_TELEPORT, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 13, RGB(13, 13, 13)
	waitforvisualfinish
	delay 12
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 13, 9, RGB(13, 13, 13)
	waitforvisualfinish
	delay 10
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 9, 5, RGB(13, 13, 13)
	waitforvisualfinish
	delay 10
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 5, 0, RGB(13, 13, 13)
	waitforvisualfinish
	clearmonbg ANIM_ATTACKER
	end

RecoverAbsorbEffect:
	createsprite gPowerAbsorptionOrbSpriteTemplate, ANIM_ATTACKER, 2, 40, -10, 13
	delay 3
	createsprite gPowerAbsorptionOrbSpriteTemplate, ANIM_ATTACKER, 2, -35, -10, 13
	delay 3
	createsprite gPowerAbsorptionOrbSpriteTemplate, ANIM_ATTACKER, 2, 15, -40, 13
	delay 3
	createsprite gPowerAbsorptionOrbSpriteTemplate, ANIM_ATTACKER, 2, -10, -32, 13
	delay 3
	createsprite gPowerAbsorptionOrbSpriteTemplate, ANIM_ATTACKER, 2, 25, -20, 13
	delay 3
	createsprite gPowerAbsorptionOrbSpriteTemplate, ANIM_ATTACKER, 2, -40, -20, 13
	delay 3
	createsprite gPowerAbsorptionOrbSpriteTemplate, ANIM_ATTACKER, 2, 5, -40, 13
	delay 3
	return

@ genanims: COPY (T-134, vision.md 9.24) approved.
Move_MIMIC:
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_CONSTRICT:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_SCRATCH, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 1, 2, 2
	delay 8
	playsewithpan SE_M_SCRATCH, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 1, 2, 2
	delay 8
	playsewithpan SE_M_SCRATCH, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 1, 2, 2
	delay 8
	playsewithpan SE_M_SCRATCH, SOUND_PAN_TARGET
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_CURSE:
	choosetwoturnanim DaemonsCurseLatent, DaemonsCurseStats
DaemonsCurseDone:
	end
DaemonsCurseLatent:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 12, RGB(13, 13, 13)
	waitforvisualfinish
	playsewithpan SE_M_NIGHTMARE, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER | F_PAL_TARGET, 1, 0, 10, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER | F_PAL_TARGET, 1, 10, 0, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER | F_PAL_TARGET, 1, 0, 10, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER | F_PAL_TARGET, 1, 10, 0, RGB(13, 13, 13)
	waitforvisualfinish
	goto DaemonsCurseDone
DaemonsCurseStats:
	playsewithpan SE_M_NIGHTMARE, SOUND_PAN_ATTACKER
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(11, 8, 14)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 6, 0, RGB(11, 8, 14)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(11, 8, 14)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 6, 0, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 6, 0, RGB(13, 13, 13)
	waitforvisualfinish
	goto DaemonsCurseDone
	end
@ DAEMONS -- REPAIR -- a fault found and fixed by hand: its colour drops part-way to grey, and each of two jolts puts
@ half of it back.
@ T-139, approved 2026-09-17 under vision.md 9.24.
Move_SOFT_BOILED:
	monbg ANIM_ATTACKER
	playsewithpan SE_M_TELEPORT, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 10, RGB(13, 13, 13)
	waitforvisualfinish
	delay 8
	playsewithpan SE_M_TAIL_WHIP, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_ShakeMon, 2, ANIM_ATTACKER, 2, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 10, 5, RGB(13, 13, 13)
	waitforvisualfinish
	delay 8
	playsewithpan SE_M_HORN_ATTACK, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_ShakeMon, 2, ANIM_ATTACKER, 2, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 5, 0, RGB(13, 13, 13)
	waitforvisualfinish
	clearmonbg ANIM_ATTACKER
	end

@ DAEMONS -- HEAL BELL -- the name was kept, and a bell is a signal to everyone: RETRAIN's side-wide reset in CONTENT,
@ struck three times to the bell's own sound. No bell or notes drawn.
@ T-139, approved 2026-09-17 under vision.md 9.24.
Move_HEAL_BELL:
	playsewithpan SE_M_HEAL_BELL, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 0, 0, 9, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 9, 0, RGB(24, 23, 21)
	waitforvisualfinish
	delay 4
	playsewithpan SE_M_HEAL_BELL, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 0, 0, 9, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 9, 0, RGB(24, 23, 21)
	waitforvisualfinish
	delay 4
	playsewithpan SE_M_HEAL_BELL, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 0, 0, 9, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 1, 9, 0, RGB(24, 23, 21)
	waitforvisualfinish
	delay 4
	end

HealBellRing:
	createvisualtask AnimTask_BlendBattleAnimPalExclude, 10, 4, 3, 8, 0, RGB(12, 24, 30)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATK_SIDE, 3, 2, 10, RGB_WHITE
	createsprite gThinRingExpandingSpriteTemplate, ANIM_ATTACKER, 40, 0, -24, 0, 1
	playsewithpan SE_M_HEAL_BELL, SOUND_PAN_ATTACKER
	return

@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_FAKE_OUT:
	playsewithpan SE_M_FLATTER, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 4, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: LOWER (T-134, vision.md 9.24) approved.
Move_SCARY_FACE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_PSYBEAM, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 5, RGB(13, 13, 13)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 5, 10, RGB(13, 13, 13)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 3, 10, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: AFFLICT (T-134, vision.md 9.24) approved.
Move_SWEET_KISS:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_HEAL_BELL, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 6, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: AFFLICT (T-134, vision.md 9.24) approved.
Move_LOVELY_KISS:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_PSYBEAM2, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 3, 0, 12, RGB(13, 13, 13)
	waitforvisualfinish
	delay 16
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 12, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_FURY_SWIPES:
	playsewithpan SE_M_SCRATCH, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ DAEMONS -- ROOT -- the process pinned where it runs: three downward jolts as it fixes in place, its colour held toward
@ GROWTH while it does, and released slowly. It will not be moved; that is the point.
@ T-139, approved 2026-09-17 under vision.md 9.24.
Move_INGRAIN:
	monbg ANIM_ATTACKER
	playsewithpan SE_M_SCRATCH, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_ShakeMon, 2, ANIM_ATTACKER, 0, 2, 2, 1
	delay 10
	playsewithpan SE_M_SCRATCH, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_ShakeMon, 2, ANIM_ATTACKER, 0, 2, 2, 1
	delay 10
	playsewithpan SE_M_SCRATCH, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_ShakeMon, 2, ANIM_ATTACKER, 0, 2, 2, 1
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 8, RGB(11, 19, 12)
	waitforvisualfinish
	delay 16
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 4, 8, 0, RGB(11, 19, 12)
	waitforvisualfinish
	clearmonbg ANIM_ATTACKER
	end

@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_PRESENT:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_TAIL_WHIP, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_BATON_PASS:
	loadspritegfx ANIM_TAG_POKEBALL
	playsewithpan SE_M_BATON_PASS, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createsprite gBatonPassPokeballSpriteTemplate, ANIM_ATTACKER, 2
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_PERISH_SONG:
	playsewithpan SE_M_PERISH_SONG, SOUND_PAN_ATTACKER
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_BATTLERS, 1, 0, 4, RGB(13, 13, 13)
	waitforvisualfinish
	delay 8
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_BATTLERS, 1, 4, 8, RGB(13, 13, 13)
	waitforvisualfinish
	delay 8
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_BATTLERS, 1, 8, 12, RGB(13, 13, 13)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_BATTLERS, 1, 12, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: COPY (T-134, vision.md 9.24) approved.
Move_SLEEP_TALK:
	playsewithpan SE_M_SNORE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 10, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 9, RGB(24, 23, 21)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 9, 9, RGB(24, 23, 21)
	waitforvisualfinish
	delay 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 3, RGB(24, 23, 21)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 3, 3, RGB(24, 23, 21)
	waitforvisualfinish
	delay 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 12, RGB(24, 23, 21)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 12, 12, RGB(24, 23, 21)
	waitforvisualfinish
	delay 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 6, RGB(24, 23, 21)
	waitforvisualfinish
	delay 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 10, RGB(24, 23, 21)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 10, 10, RGB(24, 23, 21)
	waitforvisualfinish
	delay 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 10, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_HYPER_FANG:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_BITE, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_BITE, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_TRI_ATTACK:
	playsewithpan SE_M_TRI_ATTACK, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_TRI_ATTACK, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_TRI_ATTACK, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: AFFLICT (T-134, vision.md 9.24) approved.
Move_WILL_O_WISP:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(27, 19, 5)
	waitforvisualfinish
	playsewithpan SE_M_EMBER, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 0, 12, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 12, 0, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 0, 12, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 2, 12, 0, RGB(27, 19, 5)
	waitforvisualfinish
	end
@ genanims: FIELD (T-134, vision.md 9.24) approved.
Move_ENCORE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_ENCORE2, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(13, 13, 13)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 12, 0, RGB(13, 13, 13)
	waitforvisualfinish
	playsewithpan SE_M_ENCORE2, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(13, 13, 13)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 12, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: COPY (T-134, vision.md 9.24) approved.
Move_TRICK:
	playsewithpan SE_M_SKETCH, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER | F_PAL_TARGET, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER | F_PAL_TARGET, 0, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	playsewithpan SE_M_SKETCH, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER | F_PAL_TARGET, 0, 0, 10, RGB(22, 10, 19)
	waitforvisualfinish
	delay 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER | F_PAL_TARGET, 0, 10, 0, RGB(22, 10, 19)
	waitforvisualfinish
	end
@ DAEMONS -- DEFERRED -- scheduled, not done: the colour dims a little and holds, as a job waits in a queue, and
@ comes back without anything having happened yet. The HP arrives next turn, with its message.
@ T-139, approved 2026-09-17 under vision.md 9.24.
Move_WISH:
	monbg ANIM_ATTACKER
	playsewithpan SE_M_REFLECT, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 0, 6, RGB(13, 13, 13)
	waitforvisualfinish
	delay 30
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 3, 6, 0, RGB(13, 13, 13)
	waitforvisualfinish
	clearmonbg ANIM_ATTACKER
	end

@ DAEMONS -- ENQUEUE -- one item pushed onto a queue: a single tick, a nudge, and a brief pulse of its routine's own type
@ colour (CONTENT). No orbs drawn in from outside.
@ T-139, approved 2026-09-17 under vision.md 9.24.
Move_STOCKPILE:
	monbg ANIM_ATTACKER
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_ShakeMon, 2, ANIM_ATTACKER, 0, 1, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	clearmonbg ANIM_ATTACKER
	end

StockpileAbsorb:
	createsprite gStockpileAbsorptionOrbSpriteTemplate, ANIM_ATTACKER, 2, 55, 55, 13
	delay 1
	createsprite gStockpileAbsorptionOrbSpriteTemplate, ANIM_ATTACKER, 2, -55, -55, 13
	delay 1
	createsprite gStockpileAbsorptionOrbSpriteTemplate, ANIM_ATTACKER, 2, 0, 55, 13
	delay 1
	createsprite gStockpileAbsorptionOrbSpriteTemplate, ANIM_ATTACKER, 2, 0, -55, 13
	delay 1
	createsprite gStockpileAbsorptionOrbSpriteTemplate, ANIM_ATTACKER, 2, 55, -34, 13
	delay 1
	createsprite gStockpileAbsorptionOrbSpriteTemplate, ANIM_ATTACKER, 2, 55, 34, 13
	delay 1
	createsprite gStockpileAbsorptionOrbSpriteTemplate, ANIM_ATTACKER, 2, -55, -34, 13
	delay 1
	createsprite gStockpileAbsorptionOrbSpriteTemplate, ANIM_ATTACKER, 2, -55, 34, 13
	delay 1
	return

@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_SPIT_UP:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 10, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 10, 0, RGB(13, 13, 13)
	waitforvisualfinish
	playsewithpan SE_M_TAKE_DOWN, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 6, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 13, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 13, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ DAEMONS -- DEQUEUE -- what ENQUEUE stored comes back out: the colour drains, and returns in as many ticked steps as
@ there were items queued (the move turn is the stockpile count, as vanilla's own branches read it).
@ T-139, approved 2026-09-17 under vision.md 9.24.
Move_SWALLOW:
	monbg ANIM_ATTACKER
	playsewithpan SE_M_TELEPORT, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 12, RGB(13, 13, 13)
	waitforvisualfinish
	delay 8
	jumpifmoveturn 2, DequeueTwo
	jumpifmoveturn 3, DequeueThree
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 12, 0, RGB(13, 13, 13)
	waitforvisualfinish
	goto DequeueDone
DequeueTwo:
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 12, 6, RGB(13, 13, 13)
	waitforvisualfinish
	delay 8
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(13, 13, 13)
	waitforvisualfinish
	goto DequeueDone
DequeueThree:
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 12, 8, RGB(13, 13, 13)
	waitforvisualfinish
	delay 8
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 8, 4, RGB(13, 13, 13)
	waitforvisualfinish
	delay 8
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 4, 0, RGB(13, 13, 13)
	waitforvisualfinish
DequeueDone:
	clearmonbg ANIM_ATTACKER
	end
Move_TRANSFORM:
	monbg ANIM_ATTACKER
	playsewithpan SE_M_TELEPORT, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 14, RGB(13, 13, 13)
	waitforvisualfinish
	waitplaysewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER, 48
	@ DAEMONS: in Halftone the colour comes back as the frame is taken.
	createvisualtask AnimTask_DaemonsPerspectiveFlash, 2
	createvisualtask AnimTask_TransformMon, 2, 0
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 14, 0, RGB(13, 13, 13)
	waitforvisualfinish
	clearmonbg ANIM_ATTACKER
	end

@ DAEMONS -- WARM BOOT -- powered off and on again: the daemon goes near-black, the field flashes once in CONTENT as the
@ power comes back, and its colour ramps up in one pass.
@ T-139, approved 2026-09-17 under vision.md 9.24.
Move_MORNING_SUN:
	monbg ANIM_ATTACKER
	playsewithpan SE_M_TELEPORT, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 14, RGB(3, 3, 3)
	waitforvisualfinish
	delay 16
	playsewithpan SE_M_MORNING_SUN, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 14, 0, RGB(3, 3, 3)
	waitforvisualfinish
	clearmonbg ANIM_ATTACKER
	end

MorningSunStar:
	createsprite gGreenStarSpriteTemplate, ANIM_ATTACKER, 2, 30, 640
	delay 5
	return

@ genanims: LOWER (T-134, vision.md 9.24) approved.
Move_SWEET_SCENT:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_SWEET_SCENT, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 5, RGB(13, 13, 13)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 5, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_HYPER_BEAM:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 0, 12, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_HYPER_BEAM, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 6, RGB(24, 23, 21)
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 5, 0, 10, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 14, RGB(24, 23, 21)
	waitforvisualfinish
	delay 16
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 14, 0, RGB(24, 23, 21)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 6, 0, RGB(24, 23, 21)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 12, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: AFFLICT (T-134, vision.md 9.24) approved.
Move_FLATTER:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(5, 5, 5)
	waitforvisualfinish
	playsewithpan SE_M_ENCORE2, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 10, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 10, 0, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 6, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: COPY (T-134, vision.md 9.24) approved.
Move_ROLE_PLAY:
	playsewithpan SE_M_TRI_ATTACK, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(22, 10, 19)
	waitforvisualfinish
	delay 12
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 12, RGB(22, 10, 19)
	waitforvisualfinish
	delay 12
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 12, 0, RGB(22, 10, 19)
	waitforvisualfinish
	end
@ DAEMONS -- CLEAR STATE -- a reset: one instant drop to grey and one instant return. Nothing is added; a fault is simply
@ no longer there.
@ T-139, approved 2026-09-17 under vision.md 9.24.
Move_REFRESH:
	monbg ANIM_ATTACKER
	playsewithpan SE_M_TELEPORT, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 13, RGB(13, 13, 13)
	waitforvisualfinish
	delay 6
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 13, 0, RGB(13, 13, 13)
	waitforvisualfinish
	clearmonbg ANIM_ATTACKER
	end

@ genanims: ENTROPY (T-134, vision.md 9.24) approved.
Move_BLAZE_KICK:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(27, 19, 5)
	waitforvisualfinish
	playsewithpan SE_M_FLAME_WHEEL, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 8, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 3, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 9, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 1, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 1, 7, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 7, 4, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 10, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 2, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 2, 6, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 6, 0, RGB(27, 19, 5)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_HYPER_VOICE:
	playsewithpan SE_M_COMET_PUNCH, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_DEF_SIDE, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_DEF_SIDE, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_COMET_PUNCH, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_DEF_SIDE, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_DEF_SIDE, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: STRATUM (T-134, vision.md 9.24) approved.
Move_SAND_TOMB:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 6, RGB(19, 15, 9)
	waitforvisualfinish
	playsewithpan SE_M_SAND_TOMB, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 8, RGB(19, 15, 9)
	waitforvisualfinish
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 1, 2, 2
	delay 8
	playsewithpan SE_M_SAND_TOMB, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 1, 2, 2
	delay 8
	playsewithpan SE_M_SAND_TOMB, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 1, 2, 2
	delay 8
	playsewithpan SE_M_SAND_TOMB, SOUND_PAN_TARGET
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 8, 0, RGB(19, 15, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 6, 0, RGB(19, 15, 9)
	waitforvisualfinish
	end
SandTombSwirlingDirt:
	createsprite gSwirlingDirtSpriteTemplate, ANIM_TARGET, 2, 0, 32, 528, 30, 10, 50, ANIM_TARGET
	delay 2
	createsprite gSwirlingDirtSpriteTemplate, ANIM_TARGET, 2, 0, 36, 480, 20, 13, -46, ANIM_TARGET
	delay 2
	createsprite gSwirlingDirtSpriteTemplate, ANIM_TARGET, 2, 0, 37, 576, 20, 5, 42, ANIM_TARGET
	delay 2
	createsprite gSwirlingDirtSpriteTemplate, ANIM_TARGET, 2, 0, 35, 400, 25, 8, -42, ANIM_TARGET
	delay 2
	createsprite gSwirlingDirtSpriteTemplate, ANIM_TARGET, 2, 0, 32, 512, 25, 13, 46, ANIM_TARGET
	delay 2
	createsprite gSwirlingDirtSpriteTemplate, ANIM_TARGET, 2, 0, 37, 464, 30, 12, -50, ANIM_TARGET
	delay 2
	return

@ genanims: FROZEN (T-134, vision.md 9.24) approved.
Move_SHEER_COLD:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(20, 25, 27)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(20, 25, 27)
	waitforvisualfinish
	playsewithpan SE_M_ICY_WIND, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 7, RGB(20, 25, 27)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 16, RGB(20, 25, 27)
	waitforvisualfinish
	delay 40
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 16, 0, RGB(20, 25, 27)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 7, 0, RGB(20, 25, 27)
	waitforvisualfinish
	end
@ genanims: LOGIC (T-134, vision.md 9.24) approved.
Move_ARM_THRUST:
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 4, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 8, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_SWAGGER, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(12, 15, 19)
	waitforvisualfinish
	end
@ genanims: FLOW (T-134, vision.md 9.24) approved.
Move_MUDDY_WATER:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 0, 6, RGB(8, 13, 22)
	waitforvisualfinish
	playsewithpan SE_M_WHIRLPOOL, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 4, 6, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 0, 13, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 2, 13, 0, RGB(8, 13, 22)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 2, 6, 0, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 7, RGB(13, 13, 13)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 7, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: GROWTH (T-134, vision.md 9.24) approved.
Move_BULLET_SEED:
	playsewithpan SE_M_COMET_PUNCH, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 2, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 2, 4, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 4, 0, RGB(11, 19, 12)
	waitforvisualfinish
	end
@ genanims: EMERGENT (T-134, vision.md 9.24) approved.
Move_DRAGON_CLAW:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 3, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 3, 0, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 3, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 0, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 3, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 3, 0, RGB(6, 15, 13)
	waitforvisualfinish
	playsewithpan SE_M_SACRED_FIRE2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_DEF_SIDE, 0, 0, 9, RGB(6, 15, 13)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(6, 15, 13)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_DEF_SIDE, 1, 9, 0, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 12, 0, RGB(6, 15, 13)
	waitforvisualfinish
	end
@ genanims: STRATUM (T-134, vision.md 9.24) approved.
Move_MUD_SHOT:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(19, 15, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(19, 15, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 8, RGB(19, 15, 9)
	waitforvisualfinish
	playsewithpan SE_M_WHIRLPOOL, SOUND_PAN_TARGET
	createvisualtask AnimTask_HorizontalShake, 5, (MAX_BATTLERS_COUNT + 1), 2, 8
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 3, 4, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(19, 15, 9)
	waitforvisualfinish
	delay 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 10, 0, RGB(19, 15, 9)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 8, 0, RGB(19, 15, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 7, RGB(13, 13, 13)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 7, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: HARDENED (T-134, vision.md 9.24) approved.
Move_METEOR_MASH:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 9, 9, RGB(24, 21, 12)
	waitforvisualfinish
	delay 4
	playsewithpan SE_M_BARRIER, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 5, 0, 1, 3
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 13, 13, RGB(24, 21, 12)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 0, RGB(24, 21, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 0, RGB(24, 21, 12)
	waitforvisualfinish
	playsewithpan SE_M_BARRIER, SOUND_PAN_ATTACKER
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(24, 21, 12)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 6, 0, RGB(24, 21, 12)
	waitforvisualfinish
	end
@ genanims: LOGIC (T-134, vision.md 9.24) approved.
Move_REVENGE:
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 5, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 5, 10, RGB(12, 15, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 10, 0, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 3, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 6, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 9, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_TAKE_DOWN, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 1
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(12, 15, 19)
	waitforvisualfinish
	end
@ genanims: CORRUPT (T-134, vision.md 9.24) approved.
Move_POISON_FANG:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(10, 11, 6)
	waitforvisualfinish
	playsewithpan SE_M_BITE, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 4, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 10, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 2, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 2, 7, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 7, 1, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 1, 5, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 5, 1, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 1, 0, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 3, 0, 3, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 3, 3, 0, RGB(10, 11, 6)
	waitforvisualfinish
	end
@ genanims: PROTECT (T-134, vision.md 9.24) approved.
Move_SUBSTITUTE:
	playsewithpan SE_M_ATTRACT, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 12, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 12, 0, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_MonToSubstitute, 2
	waitforvisualfinish
	end
@ genanims: GROWTH (T-134, vision.md 9.24) approved.
Move_FRENZY_PLANT:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(11, 19, 12)
	waitforvisualfinish
	playsewithpan SE_M_SCRATCH, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 5, 0, 6, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 14, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 14, 4, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 9, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 7, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 7, 0, RGB(11, 19, 12)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: LOWER (T-134, vision.md 9.24) approved.
Move_METAL_SOUND:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 21, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 21, 12)
	waitforvisualfinish
	playsewithpan SE_M_SCREECH, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 6, RGB(24, 21, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 6, 0, RGB(24, 21, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(24, 21, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(24, 21, 12)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 5, RGB(13, 13, 13)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 5, 10, RGB(13, 13, 13)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 10, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: LOGIC (T-134, vision.md 9.24) approved.
Move_FOCUS_PUNCH:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(12, 15, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 4, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 8, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 12, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_SWAGGER, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 5, 0, 7, 1
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 0, RGB(12, 15, 19)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_RETURN:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_TAIL_WHIP, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: RAISE (T-134, vision.md 9.24) approved.
Move_COSMIC_POWER:
	playsewithpan SE_M_COSMIC_POWER, SOUND_PAN_ATTACKER
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(22, 10, 19)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 6, 0, RGB(22, 10, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(22, 10, 19)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 6, 0, RGB(22, 10, 19)
	waitforvisualfinish
	end
@ genanims: ENTROPY (T-134, vision.md 9.24) approved.
Move_BLAST_BURN:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(27, 19, 5)
	waitforvisualfinish
	playsewithpan SE_M_SACRED_FIRE, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 10, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 14, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 14, 4, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 11, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 2, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 2, 8, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 4, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 12, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 2, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 2, 7, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 7, 0, RGB(27, 19, 5)
	waitforvisualfinish
	playsewithpan SE_M_SACRED_FIRE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_BATTLERS, 0, 0, 12, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG | F_PAL_BATTLERS, 2, 12, 0, RGB(27, 19, 5)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: LEGACY (T-134, vision.md 9.24) approved.
Move_ROCK_TOMB:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 0, 7, RGB(26, 26, 26)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 7, 0, RGB(26, 26, 26)
	waitforvisualfinish
	delay 6
	playsewithpan SE_M_STRENGTH, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 4, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 6, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 3, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 0, RGB(26, 26, 26)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	delay 12
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 3, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: SWARM (T-134, vision.md 9.24) approved.
Move_SILVER_WIND:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(16, 18, 9)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_GUST, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 2, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_GUST, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 2, 4, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_GUST, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 6, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_GUST, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 1, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 6, 8, RGB(16, 18, 9)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(16, 18, 9)
	waitforvisualfinish
	playsewithpan SE_M_GUST, SOUND_PAN_ATTACKER
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(16, 18, 9)
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 6, 0, RGB(16, 18, 9)
	waitforvisualfinish
	end
@ genanims: PROTECT (T-134, vision.md 9.24) approved.
Move_SNATCH:
	playsewithpan SE_M_TAKE_DOWN, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(13, 13, 13)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 10, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 10, 0, RGB(5, 5, 5)
	waitforvisualfinish
	end
@ genanims: FLOW (T-134, vision.md 9.24) approved.
Move_DIVE:
	choosetwoturnanim DaemonsDescendSink, DaemonsDescendSurface
DaemonsDescendDone:
	end
DaemonsDescendSink:
	playsewithpan SE_M_HEADBUTT, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 12, RGB(8, 13, 22)
	waitforvisualfinish
	invisible ANIM_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 12, 0, RGB(8, 13, 22)
	waitforvisualfinish
	goto DaemonsDescendDone
DaemonsDescendSurface:
	visible ANIM_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(8, 13, 22)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 3, RGB(8, 13, 22)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 7, RGB(8, 13, 22)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 7, 11, RGB(8, 13, 22)
	waitforvisualfinish
	playsewithpan SE_M_HEADBUTT, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 3, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 0, RGB(8, 13, 22)
	waitforvisualfinish
	goto DaemonsDescendDone
	end
@ genanims: LEGACY (T-134, vision.md 9.24) approved.
Move_ROCK_BLAST:
	playsewithpan SE_M_SWAGGER, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 5, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 5, 2, RGB(26, 26, 26)
	waitforvisualfinish
	delay 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 2, 0, RGB(26, 26, 26)
	waitforvisualfinish
	end
@ genanims: ENTROPY (T-134, vision.md 9.24) approved.
Move_OVERHEAT:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(27, 19, 5)
	waitforvisualfinish
	playsewithpan SE_M_DRAGON_RAGE, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 10, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 14, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 14, 4, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 11, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 2, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 2, 8, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 4, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 12, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 2, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 2, 7, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 6, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 6, 2, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 2, 5, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 5, 1, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 1, 4, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 4, 0, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 7, 0, RGB(27, 19, 5)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: FLOW (T-134, vision.md 9.24) approved.
Move_HYDRO_CANNON:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 0, 6, RGB(8, 13, 22)
	waitforvisualfinish
	playsewithpan SE_M_SURF, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 0, 5, 7, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 14, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 3, 14, 0, RGB(8, 13, 22)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 3, 6, 0, RGB(8, 13, 22)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: LATENT (T-134, vision.md 9.24) approved.
Move_ASTONISH:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 3, RGB(11, 8, 14)
	waitforvisualfinish
	playsewithpan SE_M_ENCORE, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 4, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 9, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(11, 8, 14)
	waitforvisualfinish
	end
@ genanims: LOGIC (T-134, vision.md 9.24) approved.
Move_SEISMIC_TOSS:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(12, 15, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 4, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 8, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 12, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_SKY_UPPERCUT, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 5, 1
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 0, RGB(12, 15, 19)
	waitforvisualfinish
	end
@ genanims: PROTECT (T-134, vision.md 9.24) approved.
Move_MAGIC_COAT:
	playsewithpan SE_M_BARRIER, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 11, RGB(22, 10, 19)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 11, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(22, 10, 19)
	waitforvisualfinish
	end
@ genanims: FLOW (T-134, vision.md 9.24) approved.
Move_WATER_PULSE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(8, 13, 22)
	waitforvisualfinish
	playsewithpan SE_M_BUBBLE3, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 0, RGB(8, 13, 22)
	waitforvisualfinish
	playsewithpan SE_M_BUBBLE3, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(8, 13, 22)
	waitforvisualfinish
	playsewithpan SE_M_BUBBLE3, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 5, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 5, 0, RGB(8, 13, 22)
	waitforvisualfinish
	end
@ genanims: EMERGENT (T-134, vision.md 9.24) approved.
Move_RECURSION:
	createvisualtask AnimTask_GetFuryCutterHitCount, 2
	jumpreteq 2, DaemonsRecursion2
	jumpreteq 3, DaemonsRecursion3
	jumpreteq 4, DaemonsRecursion4
	playsewithpan SE_M_PSYBEAM2, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 5, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 5, 0, RGB(6, 15, 13)
	waitforvisualfinish
	playsewithpan SE_M_PSYBEAM2, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 11, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 4, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 4, 0, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 1
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 11, RGB(6, 15, 13)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 0, RGB(6, 15, 13)
	waitforvisualfinish
	end
DaemonsRecursion2:
	playsewithpan SE_M_PSYBEAM2, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 5, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 5, 0, RGB(6, 15, 13)
	waitforvisualfinish
	playsewithpan SE_M_PSYBEAM2, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 6, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 4, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 4, 0, RGB(6, 15, 13)
	waitforvisualfinish
	playsewithpan SE_M_PSYBEAM2, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 6, 13, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 4, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 4, 0, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 6, 1
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 13, 13, RGB(6, 15, 13)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 0, RGB(6, 15, 13)
	waitforvisualfinish
	end
DaemonsRecursion3:
	playsewithpan SE_M_PSYBEAM2, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 5, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 5, 0, RGB(6, 15, 13)
	waitforvisualfinish
	playsewithpan SE_M_PSYBEAM2, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 4, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 4, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 4, 0, RGB(6, 15, 13)
	waitforvisualfinish
	playsewithpan SE_M_PSYBEAM2, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 4, 9, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 4, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 4, 0, RGB(6, 15, 13)
	waitforvisualfinish
	playsewithpan SE_M_PSYBEAM2, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 9, 15, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 4, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 4, 0, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 5, 0, 7, 1
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 15, 15, RGB(6, 15, 13)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 0, RGB(6, 15, 13)
	waitforvisualfinish
	end
DaemonsRecursion4:
	playsewithpan SE_M_PSYBEAM2, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 5, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 5, 0, RGB(6, 15, 13)
	waitforvisualfinish
	playsewithpan SE_M_PSYBEAM2, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 3, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 4, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 4, 0, RGB(6, 15, 13)
	waitforvisualfinish
	playsewithpan SE_M_PSYBEAM2, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 3, 7, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 4, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 4, 0, RGB(6, 15, 13)
	waitforvisualfinish
	playsewithpan SE_M_PSYBEAM2, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 7, 12, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 4, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 4, 0, RGB(6, 15, 13)
	waitforvisualfinish
	playsewithpan SE_M_PSYBEAM2, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 12, 16, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 0, 4, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 0, 4, 0, RGB(6, 15, 13)
	waitforvisualfinish
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 5, 0, 7, 1
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 16, 16, RGB(6, 15, 13)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 0, RGB(6, 15, 13)
	waitforvisualfinish
	end
@ genanims: CONTEXT (T-134, vision.md 9.24) approved.
Move_PSYCHO_BOOST:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 0, 9, RGB(22, 10, 19)
	waitforvisualfinish
	playsewithpan SE_M_PSYBEAM2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 7, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 0, 14, RGB(22, 10, 19)
	waitforvisualfinish
	delay 8
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 14, 0, RGB(22, 10, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 9, 0, RGB(22, 10, 19)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: OPAQUE (T-134, vision.md 9.24) approved.
Move_KNOCK_OFF:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 12, RGB(5, 5, 5)
	waitforvisualfinish
	playsewithpan SE_M_VITAL_THROW, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 2, 0, 3, 1
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 12, 0, RGB(5, 5, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 8, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
@ genanims: HARDENED (T-134, vision.md 9.24) approved.
Move_DOOM_DESIRE:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 9, 9, RGB(24, 21, 12)
	waitforvisualfinish
	delay 4
	playsewithpan SE_M_PSYBEAM, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 5, 5, RGB(24, 21, 12)
	waitforvisualfinish
	delay 20
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 0, RGB(24, 21, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 0, RGB(24, 21, 12)
	waitforvisualfinish
	end
@ genanims: LOGIC (T-134, vision.md 9.24) approved.
Move_SKY_UPPERCUT:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(12, 15, 19)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(12, 15, 19)
	waitforvisualfinish
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 4, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 4, 8, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 8, 12, RGB(12, 15, 19)
	waitforvisualfinish
	delay 2
	playsewithpan SE_M_SKY_UPPERCUT, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 5, 1
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 12, 0, RGB(12, 15, 19)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_SECRET_POWER:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_COMET_PUNCH, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 5, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
@ genanims: EMERGENT (T-134, vision.md 9.24) approved.
Move_TWISTER:
	playsewithpan SE_M_TWISTER, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 0, 10, RGB(6, 15, 13)
	waitforvisualfinish
	playsewithpan SE_M_TWISTER, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 4, 2
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_BG, 1, 10, 0, RGB(6, 15, 13)
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 1, 0, 9, RGB(6, 15, 13)
	waitforvisualfinish
	delay 6
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_DEF_SIDE, 0, 9, 0, RGB(6, 15, 13)
	waitforvisualfinish
	end
@ genanims: GROWTH (T-134, vision.md 9.24) approved.
Move_MAGICAL_LEAF:
	playsewithpan SE_M_POISON_POWDER, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 4, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 11, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 11, 3, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 3, 5, RGB(11, 19, 12)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 1, 5, 0, RGB(11, 19, 12)
	waitforvisualfinish
	end
@ genanims: FROZEN (T-134, vision.md 9.24) approved.
Move_ICE_BALL:
	playsewithpan SE_M_ICY_WIND, SOUND_PAN_TARGET
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 9, RGB(20, 25, 27)
	waitforvisualfinish
	delay 14
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 9, 0, RGB(20, 25, 27)
	waitforvisualfinish
	end
@ genanims: CONTENT (T-134, vision.md 9.24) approved.
Move_WEATHER_BALL:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 6, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 6, 0, RGB(24, 23, 21)
	waitforvisualfinish
	playsewithpan SE_M_SWAGGER, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 4, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 0, 10, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_TARGET, 0, 10, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
Move_COUNT:
	loadspritegfx ANIM_TAG_IMPACT
	monbg ANIM_TARGET
	setalpha 12, 8
	playsewithpan SE_M_DOUBLE_SLAP, SOUND_PAN_TARGET
	createsprite gBasicHitSplatSpriteTemplate, ANIM_ATTACKER, 2, 0, 0, ANIM_TARGET, 2
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 6, 1
	waitforvisualfinish
	clearmonbg ANIM_TARGET
	blendoff
	end

IceCrystalEffectShort:
	createsprite gIceCrystalHitLargeSpriteTemplate, ANIM_TARGET, 2, -10, -10, 0
	playsewithpan SE_M_ICY_WIND, SOUND_PAN_TARGET
	delay 4
	createsprite gIceCrystalHitSmallSpriteTemplate, ANIM_TARGET, 2, 10, 20, 0
	playsewithpan SE_M_ICY_WIND, SOUND_PAN_TARGET
	delay 4
	createsprite gIceCrystalHitLargeSpriteTemplate, ANIM_TARGET, 2, -5, 10, 0
	playsewithpan SE_M_ICY_WIND, SOUND_PAN_TARGET
	delay 4
	createsprite gIceCrystalHitSmallSpriteTemplate, ANIM_TARGET, 2, 17, -12, 0
	playsewithpan SE_M_ICY_WIND, SOUND_PAN_TARGET
	delay 4
	createsprite gIceCrystalHitSmallSpriteTemplate, ANIM_TARGET, 2, -15, 15, 0
	playsewithpan SE_M_ICY_WIND, SOUND_PAN_TARGET
	delay 4
	createsprite gIceCrystalHitSmallSpriteTemplate, ANIM_TARGET, 2, 0, 0, 0
	playsewithpan SE_M_ICY_WIND, SOUND_PAN_TARGET
	delay 4
	createsprite gIceCrystalHitLargeSpriteTemplate, ANIM_TARGET, 2, 20, 2, 0
	playsewithpan SE_M_ICY_WIND, SOUND_PAN_TARGET
	return

IceCrystalEffectLong:
	createsprite gIceCrystalHitLargeSpriteTemplate, ANIM_TARGET, 2, -10, -10, 1
	playsewithpan SE_M_ICY_WIND, SOUND_PAN_TARGET
	delay 4
	createsprite gIceCrystalHitSmallSpriteTemplate, ANIM_TARGET, 2, 10, 20, 1
	playsewithpan SE_M_ICY_WIND, SOUND_PAN_TARGET
	delay 4
	createsprite gIceCrystalHitLargeSpriteTemplate, ANIM_TARGET, 2, -29, 0, 1
	playsewithpan SE_M_ICY_WIND, SOUND_PAN_TARGET
	delay 4
	createsprite gIceCrystalHitSmallSpriteTemplate, ANIM_TARGET, 2, 29, -20, 1
	playsewithpan SE_M_ICY_WIND, SOUND_PAN_TARGET
	delay 4
	createsprite gIceCrystalHitLargeSpriteTemplate, ANIM_TARGET, 2, -5, 10, 1
	playsewithpan SE_M_ICY_WIND, SOUND_PAN_TARGET
	delay 4
	createsprite gIceCrystalHitSmallSpriteTemplate, ANIM_TARGET, 2, 17, -12, 1
	playsewithpan SE_M_ICY_WIND, SOUND_PAN_TARGET
	delay 4
	createsprite gIceCrystalHitLargeSpriteTemplate, ANIM_TARGET, 2, -20, 0, 1
	playsewithpan SE_M_ICY_WIND, SOUND_PAN_TARGET
	delay 4
	createsprite gIceCrystalHitSmallSpriteTemplate, ANIM_TARGET, 2, -15, 15, 1
	playsewithpan SE_M_ICY_WIND, SOUND_PAN_TARGET
	delay 4
	createsprite gIceCrystalHitSmallSpriteTemplate, ANIM_TARGET, 2, 26, -5, 1
	playsewithpan SE_M_ICY_WIND, SOUND_PAN_TARGET
	delay 4
	createsprite gIceCrystalHitSmallSpriteTemplate, ANIM_TARGET, 2, 0, 0, 1
	playsewithpan SE_M_ICY_WIND, SOUND_PAN_TARGET
	delay 4
	createsprite gIceCrystalHitLargeSpriteTemplate, ANIM_TARGET, 2, 20, 2, 1
	playsewithpan SE_M_ICY_WIND, SOUND_PAN_TARGET
	return

IceSpikesEffectShort: @ Unused
	loopsewithpan SE_M_ICY_WIND, SOUND_PAN_TARGET, 6, 4
	createsprite gIceGroundSpikeSpriteTemplate, ANIM_TARGET, 2, 0, 24, 0
	delay 4
	createsprite gIceGroundSpikeSpriteTemplate, ANIM_TARGET, 2, 8, 24, 0
	createsprite gIceGroundSpikeSpriteTemplate, ANIM_TARGET, 2, -8, 24, 0
	delay 4
	createsprite gIceGroundSpikeSpriteTemplate, ANIM_TARGET, 2, 16, 24, 0
	createsprite gIceGroundSpikeSpriteTemplate, ANIM_TARGET, 2, -16, 24, 0
	delay 4
	createsprite gIceGroundSpikeSpriteTemplate, ANIM_TARGET, 2, 24, 24, 0
	createsprite gIceGroundSpikeSpriteTemplate, ANIM_TARGET, 2, -24, 24, 0
	delay 4
	createsprite gIceGroundSpikeSpriteTemplate, ANIM_TARGET, 2, 32, 24, 0
	createsprite gIceGroundSpikeSpriteTemplate, ANIM_TARGET, 2, -32, 24, 0
	return

IceSpikesEffectLong:
	loopsewithpan SE_M_ICY_WIND, SOUND_PAN_TARGET, 6, 4
	createsprite gIceGroundSpikeSpriteTemplate, ANIM_TARGET, 2, 0, 24, 1
	delay 4
	createsprite gIceGroundSpikeSpriteTemplate, ANIM_TARGET, 2, 8, 24, 1
	createsprite gIceGroundSpikeSpriteTemplate, ANIM_TARGET, 2, -8, 24, 1
	delay 4
	createsprite gIceGroundSpikeSpriteTemplate, ANIM_TARGET, 2, 16, 24, 1
	createsprite gIceGroundSpikeSpriteTemplate, ANIM_TARGET, 2, -16, 24, 1
	delay 4
	createsprite gIceGroundSpikeSpriteTemplate, ANIM_TARGET, 2, 24, 24, 1
	createsprite gIceGroundSpikeSpriteTemplate, ANIM_TARGET, 2, -24, 24, 1
	delay 4
	createsprite gIceGroundSpikeSpriteTemplate, ANIM_TARGET, 2, 32, 24, 1
	createsprite gIceGroundSpikeSpriteTemplate, ANIM_TARGET, 2, -32, 24, 1
	delay 4
	createsprite gIceGroundSpikeSpriteTemplate, ANIM_TARGET, 2, 40, 24, 1
	createsprite gIceGroundSpikeSpriteTemplate, ANIM_TARGET, 2, -40, 24, 1
	delay 4
	createsprite gIceGroundSpikeSpriteTemplate, ANIM_TARGET, 2, 48, 24, 1
	createsprite gIceGroundSpikeSpriteTemplate, ANIM_TARGET, 2, -48, 24, 1
	return

GrantingStarsEffect:
	createsprite gGrantingStarsSpriteTemplate, ANIM_ATTACKER, 2, -15, 0, 0, 0, 32, 60
	delay 8
	createsprite gGrantingStarsSpriteTemplate, ANIM_ATTACKER, 2, 12, -5, 0, 0, 32, 60
	delay 8
	return

HealingEffect:
	playsewithpan SE_M_ABSORB_2, SOUND_PAN_ATTACKER
	createsprite gHealingBlueStarSpriteTemplate, ANIM_ATTACKER, 2, 0, -5, 0, 0
	delay 7
	createsprite gHealingBlueStarSpriteTemplate, ANIM_ATTACKER, 2, -15, 10, 0, 0
	delay 7
	createsprite gHealingBlueStarSpriteTemplate, ANIM_ATTACKER, 2, -15, -15, 0, 0
	delay 7
	createsprite gHealingBlueStarSpriteTemplate, ANIM_ATTACKER, 2, 10, -5, 0, 0
	delay 7
	return

HealingEffect2:
	playsewithpan SE_M_ABSORB_2, SOUND_PAN_TARGET
	createsprite gHealingBlueStarSpriteTemplate, ANIM_TARGET, 2, 0, -5, 1, 0
	delay 7
	createsprite gHealingBlueStarSpriteTemplate, ANIM_TARGET, 2, -15, 10, 1, 0
	delay 7
	createsprite gHealingBlueStarSpriteTemplate, ANIM_TARGET, 2, -15, -15, 1, 0
	delay 7
	createsprite gHealingBlueStarSpriteTemplate, ANIM_TARGET, 2, 10, -5, 1, 0
	delay 7
	return

PoisonBubblesEffect:
	createsprite gPoisonBubbleSpriteTemplate, ANIM_TARGET, 2, 10, 10, 0
	playsewithpan SE_M_TOXIC, SOUND_PAN_TARGET
	delay 6
	createsprite gPoisonBubbleSpriteTemplate, ANIM_TARGET, 2, 20, -20, 0
	playsewithpan SE_M_TOXIC, SOUND_PAN_TARGET
	delay 6
	createsprite gPoisonBubbleSpriteTemplate, ANIM_TARGET, 2, -20, 15, 0
	playsewithpan SE_M_TOXIC, SOUND_PAN_TARGET
	delay 6
	createsprite gPoisonBubbleSpriteTemplate, ANIM_TARGET, 2, 0, 0, 0
	playsewithpan SE_M_TOXIC, SOUND_PAN_TARGET
	delay 6
	createsprite gPoisonBubbleSpriteTemplate, ANIM_TARGET, 2, -20, -20, 0
	playsewithpan SE_M_TOXIC, SOUND_PAN_TARGET
	delay 6
	createsprite gPoisonBubbleSpriteTemplate, ANIM_TARGET, 2, 16, -8, 0
	playsewithpan SE_M_TOXIC, SOUND_PAN_TARGET
	return

WaterBubblesEffectShort:
	createsprite gWaterBubbleSpriteTemplate, ANIM_ATTACKER, 2, 10, 10, 0
	playsewithpan SE_M_BUBBLE3, SOUND_PAN_TARGET
	delay 6
	createsprite gWaterBubbleSpriteTemplate, ANIM_ATTACKER, 2, 20, -20, 0
	playsewithpan SE_M_BUBBLE3, SOUND_PAN_TARGET
	delay 6
	createsprite gWaterBubbleSpriteTemplate, ANIM_ATTACKER, 2, -20, 15, 0
	playsewithpan SE_M_BUBBLE3, SOUND_PAN_TARGET
	delay 6
	createsprite gWaterBubbleSpriteTemplate, ANIM_ATTACKER, 2, 0, 0, 0
	playsewithpan SE_M_BUBBLE3, SOUND_PAN_TARGET
	delay 6
	createsprite gWaterBubbleSpriteTemplate, ANIM_ATTACKER, 2, -20, -20, 0
	playsewithpan SE_M_BUBBLE3, SOUND_PAN_TARGET
	delay 6
	createsprite gWaterBubbleSpriteTemplate, ANIM_ATTACKER, 2, 16, -8, 0
	playsewithpan SE_M_BUBBLE3, SOUND_PAN_TARGET
	return

WaterBubblesEffectLong:
	createsprite gWaterBubbleSpriteTemplate, ANIM_ATTACKER, 2, 10, 10, 1
	playsewithpan SE_M_BUBBLE3, SOUND_PAN_TARGET
	delay 6
	createsprite gWaterBubbleSpriteTemplate, ANIM_ATTACKER, 2, -28, -10, 1
	playsewithpan SE_M_BUBBLE3, SOUND_PAN_TARGET
	delay 6
	createsprite gWaterBubbleSpriteTemplate, ANIM_ATTACKER, 2, 20, -20, 1
	playsewithpan SE_M_BUBBLE3, SOUND_PAN_TARGET
	delay 6
	createsprite gWaterBubbleSpriteTemplate, ANIM_ATTACKER, 2, -20, 15, 1
	playsewithpan SE_M_BUBBLE3, SOUND_PAN_TARGET
	delay 6
	createsprite gWaterBubbleSpriteTemplate, ANIM_ATTACKER, 2, 0, 0, 1
	playsewithpan SE_M_BUBBLE3, SOUND_PAN_TARGET
	delay 6
	createsprite gWaterBubbleSpriteTemplate, ANIM_ATTACKER, 2, 27, 8, 1
	playsewithpan SE_M_BUBBLE3, SOUND_PAN_TARGET
	delay 6
	createsprite gWaterBubbleSpriteTemplate, ANIM_ATTACKER, 2, -20, -20, 1
	playsewithpan SE_M_BUBBLE3, SOUND_PAN_TARGET
	delay 6
	createsprite gWaterBubbleSpriteTemplate, ANIM_ATTACKER, 2, 16, -8, 1
	playsewithpan SE_M_BUBBLE3, SOUND_PAN_TARGET
	return

ElectricityEffect:
	playsewithpan SE_M_THUNDERBOLT2, SOUND_PAN_TARGET
	createsprite gElectricitySpriteTemplate, ANIM_TARGET, 2, 5, 0, 5, 0
	delay 2
	createsprite gElectricitySpriteTemplate, ANIM_TARGET, 2, -5, 10, 5, 1
	delay 2
	createsprite gElectricitySpriteTemplate, ANIM_TARGET, 2, 15, 20, 5, 2
	delay 2
	createsprite gElectricitySpriteTemplate, ANIM_TARGET, 2, -15, -10, 5, 0
	delay 2
	createsprite gElectricitySpriteTemplate, ANIM_TARGET, 2, 25, 0, 5, 1
	delay 2
	createsprite gElectricitySpriteTemplate, ANIM_TARGET, 2, -8, 8, 5, 2
	delay 2
	createsprite gElectricitySpriteTemplate, ANIM_TARGET, 2, 2, -8, 5, 0
	delay 2
	createsprite gElectricitySpriteTemplate, ANIM_TARGET, 2, -20, 15, 5, 1
	return

ConfusionEffect:
	loopsewithpan SE_M_DIZZY_PUNCH, SOUND_PAN_TARGET, 13, 6
	createsprite gConfusionDuckSpriteTemplate, ANIM_TARGET, 2, 0, -15, 0, 3, 90
	createsprite gConfusionDuckSpriteTemplate, ANIM_TARGET, 2, 0, -15, 51, 3, 90
	createsprite gConfusionDuckSpriteTemplate, ANIM_TARGET, 2, 0, -15, 102, 3, 90
	createsprite gConfusionDuckSpriteTemplate, ANIM_TARGET, 2, 0, -15, 153, 3, 90
	createsprite gConfusionDuckSpriteTemplate, ANIM_TARGET, 2, 0, -15, 204, 3, 90
	return

SetPsychicBackground:
	fadetobg BG_PSYCHIC
	waitbgfadeout
	createvisualtask AnimTask_SetPsychicBackground, 5
	waitbgfadein
	return

UnsetPsychicBackground:
	restorebg
	waitbgfadeout
	setarg 7, 0xFFFF
	waitbgfadein
	return

SetSkyBg:
	jumpifcontest SetSkyBgContest
	fadetobg BG_SKY
	waitbgfadeout
	createvisualtask AnimTask_StartSlidingBg, 5, -2304, 768, 1, -1
SetSkyBgContinue:
	waitbgfadein
	return

SetSkyBgContest:
	fadetobg BG_SKY_CONTESTS
	waitbgfadeout
	createvisualtask AnimTask_StartSlidingBg, 5, 2304, 768, 0, -1
	goto SetSkyBgContinue

UnsetSkyBg:
	restorebg
	waitbgfadeout
	setarg 7, 0xFFFF
	waitbgfadein
	return

SetSolarBeamBg:
	createvisualtask AnimTask_IsContest, 2
	jumprettrue SetSolarBeamBgContest
	createvisualtask AnimTask_IsTargetPlayerSide, 2
	jumpretfalse SetSolarBeamBgOpponent
	goto SetSolarBeamBgPlayer

SetSolarBeamBgContinue:
	waitbgfadein
	return

SetSolarBeamBgContest:
	fadetobg BG_SOLAR_BEAM_CONTESTS
	goto SetSolarBeamBgContinue

SetSolarBeamBgPlayer:
	fadetobg BG_SOLAR_BEAM_PLAYER
	goto SetSolarBeamBgContinue

SetSolarBeamBgOpponent:
	fadetobg BG_SOLAR_BEAM_OPPONENT
	goto SetSolarBeamBgContinue

UnsetSolarBeamBg:
	restorebg
	waitbgfadein
	return

Status_Poison:
	playsewithpan SE_M_TOXIC, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendColorCycle, 2, F_PAL_ATTACKER, 2, 1, 0, 9, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 7, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 3, 7, 0, RGB(13, 13, 13)
	waitforvisualfinish
	delay 6
	playsewithpan SE_M_TOXIC, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendColorCycle, 2, F_PAL_ATTACKER, 2, 1, 0, 9, RGB(10, 11, 6)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 7, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 3, 7, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end

Status_Confusion:
	loopsewithpan SE_M_DIZZY_PUNCH, SOUND_PAN_ATTACKER, 13, 3
	createvisualtask AnimTask_ShakeMon2, 2, ANIM_ATTACKER, 1, 0, 28, 1
	createvisualtask AnimTask_BlendColorCycle, 2, F_PAL_ATTACKER, 0, 7, 0, 7, RGB(13, 13, 13)
	waitforvisualfinish
	end

Status_Burn:
	playsewithpan SE_M_FLAME_WHEEL, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 3, 0, 10, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendColorCycle, 2, F_PAL_ATTACKER, 1, 3, 10, 6, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 10, 0, RGB(27, 19, 5)
	waitforvisualfinish
	end

BurnFlame:
	createsprite gBurnFlameSpriteTemplate, ANIM_TARGET, 2, -24, 24, 24, 24, 20, 1, 1
	delay 4
	return

Status_Infatuation:
@ genstates: Status_Infatuation -- PAIR (T-168)
.if DAEMONS_DEBUG
	playsewithpan SE_M_CHARM, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 3, 0, 8, RGB(24, 23, 21)
	createvisualtask AnimTask_BlendBattleAnimPalExclude, 10, 2, 3, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 3, 8, 0, RGB(24, 23, 21)
	createvisualtask AnimTask_BlendBattleAnimPalExclude, 10, 2, 3, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	delay 10
	playsewithpan SE_M_CHARM, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 3, 0, 8, RGB(24, 23, 21)
	createvisualtask AnimTask_BlendBattleAnimPalExclude, 10, 2, 3, 0, 8, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 3, 8, 0, RGB(24, 23, 21)
	createvisualtask AnimTask_BlendBattleAnimPalExclude, 10, 2, 3, 8, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
.else
	loadspritegfx ANIM_TAG_MAGENTA_HEART
	playsewithpan SE_M_CHARM, SOUND_PAN_ATTACKER
	createsprite gMagentaHeartSpriteTemplate, ANIM_ATTACKER, 3, 0, 20
	delay 15
	playsewithpan SE_M_CHARM, SOUND_PAN_ATTACKER
	createsprite gMagentaHeartSpriteTemplate, ANIM_ATTACKER, 3, -20, 20
	delay 15
	playsewithpan SE_M_CHARM, SOUND_PAN_ATTACKER
	createsprite gMagentaHeartSpriteTemplate, ANIM_ATTACKER, 3, 20, 20
	end
.endif

Status_Sleep:
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 3, 0, 10, RGB(13, 13, 13)
	waitforvisualfinish
	delay 36
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 3, 10, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end

Status_Paralysis:
	playsewithpan SE_M_THUNDERBOLT2, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_ShakeMon2, 2, ANIM_ATTACKER, 1, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 9, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 9, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 6
	playsewithpan SE_M_THUNDERBOLT2, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_ShakeMon2, 2, ANIM_ATTACKER, 1, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 9, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 9, 0, RGB(10, 23, 23)
	waitforvisualfinish
	delay 30
	playsewithpan SE_M_THUNDERBOLT2, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_ShakeMon2, 2, ANIM_ATTACKER, 1, 0, 2, 1
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 9, RGB(10, 23, 23)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 9, 0, RGB(10, 23, 23)
	waitforvisualfinish
	end

Status_Freeze:
	playsewithpan SE_M_ICY_WIND, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 12, RGB(20, 25, 27)
	waitforvisualfinish
	delay 44
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 12, 0, RGB(20, 25, 27)
	waitforvisualfinish
	end

Status_Curse:
@ genstates: Status_Curse -- CONSULT (T-168)
.if DAEMONS_DEBUG
	playsewithpan SE_M_CONFUSE_RAY, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 14, RGB(13, 13, 13)
	waitforvisualfinish
	delay 16
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 14, 0, RGB(13, 13, 13)
	waitforvisualfinish
	delay 4
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 6, RGB(13, 13, 13)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 6, 0, RGB(13, 13, 13)
	waitforvisualfinish
	end
.else
	loadspritegfx ANIM_TAG_GHOSTLY_SPIRIT
	monbg ANIM_DEF_PARTNER
	playsewithpan SE_M_NIGHTMARE, SOUND_PAN_TARGET
	createsprite gCurseGhostSpriteTemplate, ANIM_TARGET, 2
	createvisualtask AnimTask_ShakeMon2, 2, ANIM_TARGET, 2, 0, 14, 1
	waitforvisualfinish
	clearmonbg ANIM_DEF_PARTNER
	end
.endif

Status_Nightmare:
@ genstates: Status_Nightmare -- STARVATION (T-168)
.if DAEMONS_DEBUG
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 3, 0, 11, RGB(11, 8, 14)
	waitforvisualfinish
	playsewithpan SE_M_NIGHTMARE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendColorCycle, 2, F_PAL_ATTACKER, 3, 2, 11, 15, RGB(11, 8, 14)
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 11, 0, RGB(11, 8, 14)
	waitforvisualfinish
	end
.else
	loadspritegfx ANIM_TAG_DEVIL
	monbg ANIM_DEF_PARTNER
	playsewithpan SE_M_NIGHTMARE, SOUND_PAN_TARGET
	createsprite gNightmareDevilSpriteTemplate, ANIM_TARGET, 2
	createvisualtask AnimTask_ShakeMon2, 2, ANIM_TARGET, 2, 0, 14, 1
	waitforvisualfinish
	clearmonbg ANIM_DEF_PARTNER
	end
.endif

General_CastformChange:
	createvisualtask AnimTask_IsMonInvisible, 2
	jumpreteq TRUE, CastformChangeSkipAnim
	goto CastformChangeContinue

CastformChangeContinue:
	monbg ANIM_ATTACKER
	playsewithpan SE_M_TELEPORT, SOUND_PAN_ATTACKER
	waitplaysewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER, 48
	createvisualtask AnimTask_TransformMon, 2, 1
	waitforvisualfinish
	clearmonbg ANIM_ATTACKER
	end

CastformChangeSkipAnim:
	createvisualtask AnimTask_CastformGfxChange, 2, 1
	end

General_StatsChange:
	createvisualtask AnimTask_StatsChange, 5
	waitforvisualfinish
	end

General_SubstituteFade:
	monbg ANIM_ATTACKER
	createvisualtask AnimTask_SubstituteFadeToInvisible, 5
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 16, RGB_WHITE
	waitforvisualfinish
	delay 1
	clearmonbg ANIM_ATTACKER
	delay 2
	blendoff
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 0, RGB_WHITE
	createvisualtask AnimTask_SwapMonSpriteToFromSubstitute, 2, TRUE
	end

General_SubstituteAppear:
	createvisualtask AnimTask_MonToSubstitute, 2
	end

General_BaitThrow:
	createvisualtask AnimTask_SafariOrGhost_DecideAnimSides, 2, 0
	createvisualtask AnimTask_LoadBaitGfx, 2
	delay 0
	waitplaysewithpan SE_M_JUMP_KICK, SOUND_PAN_ATTACKER, 22
	createsprite gSafariBaitSpriteTemplate, ANIM_TARGET, 3, -18, 12, 0, 32
	delay 50
	loopsewithpan SE_M_TAIL_WHIP, SOUND_PAN_TARGET, 19, 2
	createvisualtask AnimTask_SwayMon, 5, 1, 8, 1536, 2, ANIM_TARGET
	waitforvisualfinish
	createvisualtask AnimTask_FreeBaitGfx, 2
	end

General_ItemKnockoff:
	loadspritegfx ANIM_TAG_ITEM_BAG
	createsprite gKnockOffItemSpriteTemplate, ANIM_TARGET, 2
	end

General_TurnTrap:
	createvisualtask AnimTask_GetTrappedMoveAnimId, 5
	jumpargeq 0, TRAP_ANIM_FIRE_SPIN, Status_FireSpin
	jumpargeq 0, TRAP_ANIM_WHIRLPOOL, Status_Whirlpool
	jumpargeq 0, TRAP_ANIM_CLAMP,     Status_Clamp
	jumpargeq 0, TRAP_ANIM_SAND_TOMB, Status_SandTomb
	goto Status_BindWrap

Status_BindWrap:
@ genstates: Status_BindWrap -- LATCH / ENCLOSE (T-168)
.if DAEMONS_DEBUG
	playsewithpan SE_M_BIND, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 9, RGB(24, 23, 21)
	waitforvisualfinish
	createvisualtask AnimTask_ShakeMon2, 2, ANIM_ATTACKER, 1, 0, 2, 2
	delay 16
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 9, 0, RGB(24, 23, 21)
	waitforvisualfinish
	end
.else
	loadspritegfx ANIM_TAG_TENDRILS
	loopsewithpan SE_M_SCRATCH, SOUND_PAN_TARGET, 6, 2
	createsprite gConstrictBindingSpriteTemplate, ANIM_TARGET, 4, 0, 16, 0, 1
	delay 7
	createsprite gConstrictBindingSpriteTemplate, ANIM_TARGET, 2, 0, 8, 1, 1
	delay 3
	createvisualtask AnimTask_ShakeMon2, 2, ANIM_TARGET, 2, 0, 8, 1
	delay 20
	setarg 7, 0xFFFF
	playsewithpan SE_M_BIND, SOUND_PAN_TARGET
	waitforvisualfinish
	end
.endif

Status_FireSpin:
@ genstates: Status_FireSpin -- THERMAL (T-168)
.if DAEMONS_DEBUG
	playsewithpan SE_M_FLAME_WHEEL, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 9, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_BlendColorCycle, 2, F_PAL_ATTACKER, 1, 3, 9, 5, RGB(27, 19, 5)
	waitforvisualfinish
	createvisualtask AnimTask_ShakeMon2, 2, ANIM_ATTACKER, 1, 0, 2, 2
	delay 16
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 9, 0, RGB(27, 19, 5)
	waitforvisualfinish
	end
.else
	loadspritegfx ANIM_TAG_SMALL_EMBER
	playsewithpan SE_M_SACRED_FIRE2, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 5, ANIM_TARGET, 0, 2, 30, 1
	call FireSpinEffect
	call FireSpinEffect
	waitforvisualfinish
	stopsound
	end
.endif

Status_Whirlpool:
@ genstates: Status_Whirlpool -- WHIRLPOOL (T-168)
.if DAEMONS_DEBUG
	playsewithpan SE_M_WHIRLPOOL, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 1, 0, 11, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_BlendColorCycle, 2, F_PAL_ATTACKER, 0, 5, 11, 5, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_ShakeMon2, 2, ANIM_ATTACKER, 1, 0, 2, 2
	delay 16
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 2, 11, 0, RGB(8, 13, 22)
	waitforvisualfinish
	end
.else
	loadspritegfx ANIM_TAG_WATER_ORB
	monbg ANIM_DEF_PARTNER
	splitbgprio ANIM_TARGET
	setalpha 12, 8
	delay 0
	createsprite gSimplePaletteBlendSpriteTemplate, ANIM_ATTACKER, 0, F_PAL_TARGET, 2, 0, 7, RGB(0, 13, 23)
	playsewithpan SE_M_WHIRLPOOL, SOUND_PAN_TARGET
	createvisualtask AnimTask_ShakeMon, 5, ANIM_TARGET, 0, 2, 30, 1
	call WhirlpoolEffect
	call WhirlpoolEffect
	delay 12
	createsprite gSimplePaletteBlendSpriteTemplate, ANIM_ATTACKER, 0, F_PAL_TARGET, 2, 7, 0, RGB(0, 13, 23)
	waitforvisualfinish
	stopsound
	clearmonbg ANIM_DEF_PARTNER
	end
.endif

Status_Clamp:
@ genstates: Status_Clamp -- SLUICE (T-168)
.if DAEMONS_DEBUG
	playsewithpan SE_M_VICEGRIP, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 0, 11, RGB(8, 13, 22)
	waitforvisualfinish
	createvisualtask AnimTask_ShakeMon2, 2, ANIM_ATTACKER, 1, 0, 2, 2
	delay 20
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 0, 11, 0, RGB(8, 13, 22)
	waitforvisualfinish
	end
.else
	loadspritegfx ANIM_TAG_CLAMP
	loadspritegfx ANIM_TAG_IMPACT
	monbg ANIM_TARGET
	setalpha 12, 8
	playsewithpan SE_M_VICEGRIP, SOUND_PAN_TARGET
	createsprite gClampJawSpriteTemplate, ANIM_ATTACKER, 2, -32, 0, 2, 819, 0, 10
	createsprite gClampJawSpriteTemplate, ANIM_ATTACKER, 2, 32, 0, 6, -819, 0, 10
	delay 10
	createsprite gBasicHitSplatSpriteTemplate, ANIM_ATTACKER, 2, 0, 0, ANIM_TARGET, 2
	createvisualtask AnimTask_ShakeMon, 5, ANIM_TARGET, 3, 0, 5, 1
	waitforvisualfinish
	clearmonbg ANIM_TARGET
	blendoff
	waitforvisualfinish
	end
.endif

Status_SandTomb:
@ genstates: Status_SandTomb -- BURY (T-168)
.if DAEMONS_DEBUG
	playsewithpan SE_M_SAND_TOMB, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 4, 0, 10, RGB(19, 15, 9)
	waitforvisualfinish
	createvisualtask AnimTask_ShakeMon2, 2, ANIM_ATTACKER, 1, 0, 2, 2
	delay 12
	waitforvisualfinish
	createvisualtask AnimTask_BlendBattleAnimPal, 10, F_PAL_ATTACKER, 4, 10, 0, RGB(19, 15, 9)
	waitforvisualfinish
	end
.else
	loadspritegfx ANIM_TAG_MUD_SAND
	createsprite gSimplePaletteBlendSpriteTemplate, ANIM_ATTACKER, 0, F_PAL_TARGET, 2, 0, 7, RGB(19, 17, 0)
	createvisualtask AnimTask_ShakeMon, 5, ANIM_TARGET, 0, 2, 30, 1
	playsewithpan SE_M_SAND_TOMB, SOUND_PAN_TARGET
	call SandTombSwirlingDirt
	call SandTombSwirlingDirt
	delay 22
	createsprite gSimplePaletteBlendSpriteTemplate, ANIM_ATTACKER, 0, F_PAL_TARGET, 2, 7, 0, RGB(19, 17, 0)
	waitforvisualfinish
	stopsound
	end
.endif

General_HeldItemEffect:
	loadspritegfx ANIM_TAG_THIN_RING
	loadspritegfx ANIM_TAG_SPARKLE_2
	delay 0
	playsewithpan SE_M_TAKE_DOWN, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_RotateMonToSideAndRestore, 2, 16, 128, ANIM_ATTACKER, 2
	waitforvisualfinish
	playsewithpan SE_M_TAKE_DOWN, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_RotateMonToSideAndRestore, 2, 16, 128, ANIM_ATTACKER, 2
	waitforvisualfinish
	playsewithpan SE_M_TAKE_DOWN, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_RotateMonToSideAndRestore, 2, 16, 128, ANIM_ATTACKER, 2
	waitforvisualfinish
	playsewithpan SE_M_MORNING_SUN, SOUND_PAN_ATTACKER
	call GrantingStarsEffect
	waitforvisualfinish
	playsewithpan SE_SHINY, SOUND_PAN_ATTACKER
	createsprite gSimplePaletteBlendSpriteTemplate, ANIM_ATTACKER, 2, F_PAL_ATTACKER, 3, 7, 0, RGB(17, 31, 25)
	createsprite gThinRingExpandingSpriteTemplate, ANIM_ATTACKER, 3, 0, 0, 0, 0
	waitforvisualfinish
	end

General_SmokeballEscape:
	loadspritegfx ANIM_TAG_PINK_CLOUD
	monbg ANIM_ATTACKER
	setalpha 12, 4
	delay 0
	playsewithpan SE_BALL_OPEN, SOUND_PAN_TARGET
	createsprite gSmokeBallEscapeCloudSpriteTemplate, ANIM_TARGET, 0, 0, 32, 28, 30
	delay 4
	playsewithpan SE_BALL_OPEN, SOUND_PAN_TARGET
	createsprite gSmokeBallEscapeCloudSpriteTemplate, ANIM_ATTACKER, 127, 2, 12, 20, 30
	delay 12
	playsewithpan SE_BALL_OPEN, SOUND_PAN_TARGET
	createsprite gSmokeBallEscapeCloudSpriteTemplate, ANIM_ATTACKER, 126, 2, -28, 4, 30
	delay 12
	playsewithpan SE_BALL_OPEN, SOUND_PAN_TARGET
	createsprite gSmokeBallEscapeCloudSpriteTemplate, ANIM_ATTACKER, 124, 2, 14, -20, 30
	delay 4
	playsewithpan SE_BALL_OPEN, SOUND_PAN_TARGET
	createvisualtask AnimTask_AttackerFadeToInvisible, 2, 2
	createsprite gSmokeBallEscapeCloudSpriteTemplate, ANIM_ATTACKER, 123, 3, 4, 4, 30
	delay 14
	playsewithpan SE_BALL_OPEN, SOUND_PAN_TARGET
	createsprite gSmokeBallEscapeCloudSpriteTemplate, ANIM_ATTACKER, 122, 3, -14, 18, 46
	delay 0
	createsprite gSmokeBallEscapeCloudSpriteTemplate, ANIM_ATTACKER, 121, 3, 14, -14, 46
	delay 0
	createsprite gSmokeBallEscapeCloudSpriteTemplate, ANIM_ATTACKER, 120, 3, -12, -10, 46
	delay 0
	createsprite gSmokeBallEscapeCloudSpriteTemplate, ANIM_ATTACKER, 119, 3, 14, 14, 46
	delay 0
	createsprite gSmokeBallEscapeCloudSpriteTemplate, ANIM_ATTACKER, 118, 3, 0, 0, 46
	waitforvisualfinish
	clearmonbg ANIM_ATTACKER
	invisible ANIM_ATTACKER
	delay 0
	blendoff
	end

General_FocusBand:
	createsprite gSimplePaletteBlendSpriteTemplate, ANIM_ATTACKER, 0, F_PAL_ATTACKER, 7, 0, 9, RGB_RED
	playsewithpan SE_M_DRAGON_RAGE, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_SlideMonForFocusBand, 5, 30, 128, 0, 1, 2, 0, 1
	waitforvisualfinish
	createsprite gSimplePaletteBlendSpriteTemplate, ANIM_ATTACKER, 0, F_PAL_ATTACKER, 4, 9, 0, RGB_RED
	waitforvisualfinish
	delay 6
	createsprite gSlideMonToOriginalPosSpriteTemplate, ANIM_ATTACKER, 0, 0, 0, 15
	end

General_Rain:
@ DAEMONS (T-144): rain that goes on plays RAIN DANCE's animation, as sun, sandstorm and hail already play theirs,
@ so the four weathers speak one language every turn they last.
	goto Move_RAIN_DANCE

General_Sun:
	goto Move_SUNNY_DAY

General_Sandstorm:
	goto Move_SANDSTORM

General_Hail:
	goto Move_HAIL

General_LeechSeedDrain:
	createvisualtask AnimTask_GetBattlersFromArg, 5
	delay 0
	goto Move_ABSORB

General_MonHit:
	loadspritegfx ANIM_TAG_IMPACT
	monbg ANIM_TARGET
	setalpha 12, 8
	playsewithpan SE_M_DOUBLE_SLAP, SOUND_PAN_TARGET
	createsprite gBasicHitSplatSpriteTemplate, ANIM_ATTACKER, 2, 0, 0, ANIM_TARGET, 2
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 3, 0, 6, 1
	waitforvisualfinish
	clearmonbg ANIM_TARGET
	blendoff
	end

General_ItemSteal:
	loadspritegfx ANIM_TAG_ITEM_BAG
	createvisualtask AnimTask_SetAnimAttackerAndTargetForEffectAtk, 2
	createvisualtask AnimTask_SetTargetToEffectBattler, 2  @ Redundant with above
	delay 1
	createsprite gItemStealSpriteTemplate, ANIM_ATTACKER, 2, 0, -5, 10, 2, -1
	end

General_SnatchMove:
	loadspritegfx ANIM_TAG_ITEM_BAG
	createvisualtask AnimTask_SetAnimAttackerAndTargetForEffectTgt, 2
	call SnatchMoveTrySwapFromSubstitute
	delay 1
	createvisualtask AnimTask_SwayMon, 2, 0, 5, 5120, 4, ANIM_TARGET
	waitforvisualfinish
	createvisualtask AnimTask_IsTargetSameSide, 2
	jumpretfalse SnatchOpposingMonMove
	goto SnatchPartnerMonMove

SnatchMoveContinue:
	waitforvisualfinish
	call SnatchMoveTrySwapToSubstitute
	end

SnatchOpposingMonMove:
	playsewithpan SE_M_DOUBLE_TEAM, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_SnatchOpposingMonMove, 2
	goto SnatchMoveContinue

SnatchPartnerMonMove:
	playsewithpan SE_M_DOUBLE_TEAM, SOUND_PAN_ATTACKER
	createvisualtask AnimTask_SnatchPartnerMove, 2
	goto SnatchMoveContinue

General_FutureSightHit:
	createvisualtask AnimTask_SetAnimTargetToBattlerTarget, 2
	monbg ANIM_DEF_PARTNER
	playsewithpan SE_M_PSYBEAM, SOUND_PAN_ATTACKER
	call SetPsychicBackground
	setalpha 8, 8
	playsewithpan SE_M_SUPERSONIC, SOUND_PAN_TARGET
	waitplaysewithpan SE_M_SUPERSONIC, SOUND_PAN_TARGET, 8
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 15, 1
	createvisualtask AnimTask_ScaleMonAndRestore, 5, -5, -5, 15, ANIM_TARGET, 1
	waitforvisualfinish
	createvisualtask AnimTask_ShakeMon, 2, ANIM_TARGET, 4, 0, 24, 1
	waitforvisualfinish
	clearmonbg ANIM_DEF_PARTNER
	blendoff
	waitforvisualfinish
	delay 1
	call UnsetPsychicBackground
	end

General_DoomDesireHit:
	createvisualtask AnimTask_SetAnimTargetToBattlerTarget, 2
	loadspritegfx ANIM_TAG_EXPLOSION
	createsprite gSimplePaletteBlendSpriteTemplate, ANIM_ATTACKER, 2, F_PAL_BG, 3, 0, 16, RGB_WHITE
	waitforvisualfinish
	delay 10
	createvisualtask AnimTask_DoomDesireLightBeam, 5
	delay 9
	playsewithpan SE_M_CONFUSE_RAY, SOUND_PAN_ATTACKER
	delay 9
	playsewithpan SE_M_CONFUSE_RAY, 0
	delay 9
	playsewithpan SE_M_CONFUSE_RAY, SOUND_PAN_TARGET
	delay 25
	createvisualtask AnimTask_ShakeMonInPlace, 2, ANIM_TARGET, 10, 0, 20, 1
	playsewithpan SE_M_SELF_DESTRUCT, SOUND_PAN_TARGET
	createsprite gExplosionSpriteTemplate, ANIM_ATTACKER, 3, 0, 0, 1, 1
	delay 6
	playsewithpan SE_M_SELF_DESTRUCT, SOUND_PAN_TARGET
	createsprite gExplosionSpriteTemplate, ANIM_ATTACKER, 3, 24, -24, 1, 1
	delay 6
	playsewithpan SE_M_SELF_DESTRUCT, SOUND_PAN_TARGET
	createsprite gExplosionSpriteTemplate, ANIM_ATTACKER, 3, -16, 16, 1, 1
	delay 6
	playsewithpan SE_M_SELF_DESTRUCT, SOUND_PAN_TARGET
	createsprite gExplosionSpriteTemplate, ANIM_ATTACKER, 3, -24, -12, 1, 1
	delay 6
	playsewithpan SE_M_SELF_DESTRUCT, SOUND_PAN_TARGET
	createsprite gExplosionSpriteTemplate, ANIM_ATTACKER, 3, 16, 16, 1, 1
	waitforvisualfinish
	createsprite gSimplePaletteBlendSpriteTemplate, ANIM_ATTACKER, 2, F_PAL_BG, 3, 16, 0, RGB_WHITE
	waitforvisualfinish
	end

General_FocusPunchSetUp:
	loadspritegfx ANIM_TAG_FOCUS_ENERGY
	playsewithpan SE_M_DRAGON_RAGE, SOUND_PAN_ATTACKER
	call EndureEffect
	delay 8
	createvisualtask AnimTask_BlendColorCycle, 2, F_PAL_ATTACKER, 2, 2, 0, 11, RGB_RED
	createvisualtask AnimTask_ShakeMon2, 2, ANIM_ATTACKER, 1, 0, 32, 1
	call EndureEffect
	delay 8
	call EndureEffect
	waitforvisualfinish
	end

General_IngrainHeal:
	loadspritegfx ANIM_TAG_ORBS
	loadspritegfx ANIM_TAG_BLUE_STAR
	monbg ANIM_DEF_PARTNER
	setalpha 12, 8
	createsprite gSimplePaletteBlendSpriteTemplate, ANIM_ATTACKER, 2, F_PAL_BG, 1, 0, 4, RGB(13, 31, 12)
	waitforvisualfinish
	delay 3
	call AbsorbEffect
	waitforvisualfinish
	delay 15
	call HealingEffect
	waitforvisualfinish
	createsprite gSimplePaletteBlendSpriteTemplate, ANIM_ATTACKER, 2, F_PAL_BG, 1, 4, 0, RGB(13, 31, 12)
	waitforvisualfinish
	clearmonbg ANIM_DEF_PARTNER
	blendoff
	end

General_WishHeal:
	loadspritegfx ANIM_TAG_BLUE_STAR
	loadspritegfx ANIM_TAG_SPARKLE_2
	createsprite gSimplePaletteBlendSpriteTemplate, ANIM_ATTACKER, 2, F_PAL_BG, 3, 0, 10, RGB_BLACK
	waitforvisualfinish
	playsewithpan SE_M_MEGA_KICK, SOUND_PAN_ATTACKER
	call GrantingStarsEffect
	waitforvisualfinish
	call HealingEffect
	waitforvisualfinish
	createsprite gSimplePaletteBlendSpriteTemplate, ANIM_ATTACKER, 2, F_PAL_BG, 3, 10, 0, RGB_BLACK
	end

General_MonScared:
	createvisualtask AnimTask_SafariOrGhost_DecideAnimSides, 2, 1
	waitforvisualfinish
	loadspritegfx ANIM_TAG_SWEAT_BEAD
	createsprite gSimplePaletteBlendSpriteTemplate, ANIM_ATTACKER, 0, F_PAL_TARGET, 2, 0, 10, RGB(0, 23, 25)
	createvisualtask AnimTask_ShakeMon2, 2, ANIM_TARGET, 2, 0, 10, 1
	delay 20
	createsprite gSprayWaterDropletSpriteTemplate, ANIM_TARGET, 5, 0, 1
	playsewithpan SE_M_SKETCH, SOUND_PAN_TARGET
	createsprite gSprayWaterDropletSpriteTemplate, ANIM_TARGET, 5, 1, 1
	createvisualtask AnimTask_ShakeMon2, 2, ANIM_TARGET, 4, 0, 5, 1
	createvisualtask AnimTask_StretchTargetUp, 3
	waitforvisualfinish
	createsprite gSimplePaletteBlendSpriteTemplate, ANIM_ATTACKER, 0, F_PAL_TARGET, 2, 10, 0, RGB(0, 23, 25)
	waitforvisualfinish
	end

General_GhostGetOut:
	createvisualtask AnimTask_SafariOrGhost_DecideAnimSides, 2, 1
	waitforvisualfinish
	fadetobg BG_GHOST
	waitbgfadeout
	monbg_static ANIM_ATTACKER
	createvisualtask AnimTask_GhostGetOut, 2
	waitbgfadein
	loopsewithpan SE_M_PSYBEAM, SOUND_PAN_TARGET, 20, 3
	waitforvisualfinish
	clearmonbg_static ANIM_ATTACKER
	delay 1
	loadspritegfx ANIM_TAG_SWEAT_BEAD
	createsprite gSimplePaletteBlendSpriteTemplate, ANIM_ATTACKER, 0, F_PAL_TARGET, -1, 0, 6, RGB(21, 22, 26)
	createsprite gSprayWaterDropletSpriteTemplate, ANIM_TARGET, 5, 0, 1
	createsprite gSprayWaterDropletSpriteTemplate, ANIM_TARGET, 5, 1, 1
	createvisualtask AnimTask_ShakeMon2, 2, ANIM_TARGET, 4, 0, 5, 1
	createvisualtask AnimTask_StretchTargetUp, 3
	waitforvisualfinish
	createsprite gSimplePaletteBlendSpriteTemplate, ANIM_ATTACKER, 0, F_PAL_TARGET, -1, 6, 0, RGB(21, 22, 26)
	waitforvisualfinish
	restorebg
	waitbgfadein
	end

General_SilphScoped:
	monbg ANIM_ATTACKER
	playsewithpan SE_M_TELEPORT, SOUND_PAN_ATTACKER
	waitplaysewithpan SE_M_MINIMIZE, SOUND_PAN_ATTACKER, 48
	createvisualtask AnimTask_TransformMon, 2, 255
	waitsound
	waitforvisualfinish
	clearmonbg ANIM_ATTACKER
	end

General_SafariRockThrow:
	createvisualtask AnimTask_SafariOrGhost_DecideAnimSides, 2, 0
	waitforvisualfinish
	loadspritegfx ANIM_TAG_ROCKS
	loadspritegfx ANIM_TAG_IMPACT
	delay 0
	waitplaysewithpan SE_M_JUMP_KICK, SOUND_PAN_ATTACKER, 22
	createsprite gSafariRockTemplate, ANIM_TARGET, 3, -17, 14, 8, 0
	delay 50
	monbg ANIM_DEF_PARTNER
	setalpha 12, 8
	delay 0
	playsewithpan SE_M_DOUBLE_SLAP, SOUND_PAN_TARGET
	createsprite gBasicHitSplatSpriteTemplate, ANIM_TARGET, 2, -4, -20, 1, 2
	waitforvisualfinish
	clearmonbg ANIM_DEF_PARTNER
	blendoff
	waitforvisualfinish
	end

General_SafariReaction:
	createvisualtask AnimTask_SafariGetReaction, 2
	waitforvisualfinish
	jumpreteq B_MSG_MON_WATCHING, SafariReaction_WatchingCarefully
	jumpreteq B_MSG_MON_ANGRY, SafariReaction_Angry
	jumpreteq B_MSG_MON_EATING, SafariReaction_Eating
	end

SafariReaction_WatchingCarefully:
	playsewithpan SE_M_TAKE_DOWN, SOUND_PAN_TARGET
	createvisualtask AnimTask_RotateMonToSideAndRestore, 2, 16, 96, 0, 2
	waitforvisualfinish
	playsewithpan SE_M_TAKE_DOWN, SOUND_PAN_TARGET
	createvisualtask AnimTask_RotateMonToSideAndRestore, 2, 16, -96, 0, 2
	end

SafariReaction_Angry:
	loadspritegfx ANIM_TAG_ANGER
	createsprite gAngerMarkSpriteTemplate, ANIM_TARGET, 2, 1, 20, -20
	playsewithpan SE_M_SWAGGER2, SOUND_PAN_TARGET
	waitforvisualfinish
	delay 12
	createsprite gAngerMarkSpriteTemplate, ANIM_TARGET, 2, 1, -20, -20
	playsewithpan SE_M_SWAGGER2, SOUND_PAN_TARGET
	end

SafariReaction_Eating:
	playsewithpan SE_M_TAKE_DOWN, SOUND_PAN_TARGET
	createvisualtask AnimTask_RotateMonToSideAndRestore, 2, 8, 136, 0, 2
	waitforvisualfinish
	playsewithpan SE_M_TAKE_DOWN, SOUND_PAN_TARGET
	createvisualtask AnimTask_RotateMonToSideAndRestore, 2, 8, 136, 0, 2
	end

SnatchMoveTrySwapFromSubstitute:
	createvisualtask AnimTask_IsAttackerBehindSubstitute, 2
	jumprettrue SnatchMoveSwapSubstituteForMon
SnatchMoveTrySwapFromSubstituteEnd:
	waitforvisualfinish
	return

SnatchMoveSwapSubstituteForMon:
	createvisualtask AnimTask_SwapMonSpriteToFromSubstitute, 2, TRUE
	waitforvisualfinish
	goto SnatchMoveTrySwapFromSubstituteEnd

SnatchMoveTrySwapToSubstitute:
	createvisualtask AnimTask_IsAttackerBehindSubstitute, 2
	jumprettrue SnatchMoveSwapMonForSubstitute
SnatchMoveTrySwapToSubstituteEnd:
	waitforvisualfinish
	return

SnatchMoveSwapMonForSubstitute:
	createvisualtask AnimTask_SwapMonSpriteToFromSubstitute, 2, FALSE
	waitforvisualfinish
	goto SnatchMoveTrySwapToSubstituteEnd

@ Healthbox blue flash effect on level up
Special_LevelUp:
	playsewithpan SE_EXP_MAX, 0
	createvisualtask AnimTask_LoadHealthboxPalsForLevelUp, 2
	delay 0
	createvisualtask AnimTask_FlashHealthboxOnLevelUp, 5, 0, 0
	waitforvisualfinish
	createvisualtask AnimTask_FreeHealthboxPalsForLevelUp, 2
	end

Special_SwitchOutPlayerMon:
	createvisualtask AnimTask_SwitchOutBallEffect, 2
	delay 10
	createvisualtask AnimTask_SwitchOutShrinkMon, 2
	end

Special_SwitchOutOpponentMon:
	createvisualtask AnimTask_SwitchOutBallEffect, 2
	delay 10
	createvisualtask AnimTask_SwitchOutShrinkMon, 2
	end

Special_BallThrow:
	createvisualtask AnimTask_LoadBallGfx, 2
	delay 0
	playsewithpan SE_BALL_THROW, 0
	createvisualtask AnimTask_ThrowBall, 2
	createvisualtask AnimTask_IsBallBlockedByTrainerOrDodged, 2
	jumpreteq -1, BallThrowTrainerBlock
	jumpreteq -2, BallThrowGhostDodged
BallThrowEnd:
	waitforvisualfinish
	createvisualtask AnimTask_FreeBallGfx, 2
	end

BallThrowTrainerBlock:
	loadspritegfx ANIM_TAG_IMPACT
	delay 25
	monbg ANIM_DEF_PARTNER
	setalpha 12, 8
	delay 0
	playsewithpan SE_M_DOUBLE_SLAP, SOUND_PAN_TARGET
	createsprite gBasicHitSplatSpriteTemplate, ANIM_TARGET, 2, -4, -20, ANIM_TARGET, 2
	waitforvisualfinish
	clearmonbg ANIM_DEF_PARTNER
	blendoff
	goto BallThrowEnd

BallThrowGhostDodged:
	delay 16
	createvisualtask AnimTask_WindUpLunge, 2, ANIM_TARGET, 48, 6, 16, 48, -48, 16
	playsewithpan SE_M_TAKE_DOWN, SOUND_PAN_TARGET
	waitplaysewithpan SE_M_TAKE_DOWN, SOUND_PAN_TARGET, 48
	waitforvisualfinish
	goto BallThrowEnd

Special_BallThrowWithTrainer:
	createvisualtask AnimTask_LoadBallGfx, 2
	delay 0
	createvisualtask AnimTask_ThrowBallSpecial, 2
	waitforvisualfinish
	createvisualtask AnimTask_FreeBallGfx, 2
	end

Special_SubstituteToMon:
	createvisualtask AnimTask_SwapMonSpriteToFromSubstitute, 2, TRUE
	end

Special_MonToSubstitute:
	createvisualtask AnimTask_SwapMonSpriteToFromSubstitute, 2, FALSE
	end
