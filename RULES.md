# Hanafuda Rules & How to Play Guide

Welcome to **SDLHana**, the modernized C++23 / SDL3 cross-platform Hanafuda game. This guide provides an illustrated overview of traditional Hanafuda card mechanics, rules, and scoring combinations for both **Japanese Koi-Koi (こいこい)** and **Korean Go-Stop (고스톱)** modes.

---

## 1. Deck Overview & Turn Mechanics

### The Hanafuda Deck

A standard Hanafuda (*Flower Cards*) deck consists of **48 cards** divided into **12 suit months** representing Japanese flora and fauna, with **4 cards per month**:

#### January (Pine / 松)
<p align="center">
  <img src="tools/cards_trad/00_Hanafuda%20January%20Hikari%20Alt.svg.png" width="90" alt="Jan Crane">
  <img src="tools/cards_trad/01_Hanafuda%20January%20Tanzaku%20Alt.svg.png" width="90" alt="Jan Ribbon">
  <img src="tools/cards_trad/02_Hanafuda%20January%20Kasu%201%20Alt.svg.png" width="90" alt="Jan Plain 1">
  <img src="tools/cards_trad/03_Hanafuda%20January%20Kasu%202%20Alt.svg.png" width="90" alt="Jan Plain 2">
</p>
- **Crane & Sun (Bright)**, **Red Poetry Ribbon**, 2 Plain Cards.

#### February (Plum Blossom / 梅)
<p align="center">
  <img src="tools/cards_trad/04_Hanafuda%20February%20Tane%20Alt.svg.png" width="90" alt="Feb Bush Warbler">
  <img src="tools/cards_trad/05_Hanafuda%20February%20Tanzaku%20Alt.svg.png" width="90" alt="Feb Ribbon">
  <img src="tools/cards_trad/06_Hanafuda%20February%20Kasu%201%20Alt.svg.png" width="90" alt="Feb Plain 1">
  <img src="tools/cards_trad/07_Hanafuda%20February%20Kasu%202%20Alt.svg.png" width="90" alt="Feb Plain 2">
</p>
- **Bush Warbler (Animal)**, **Red Poetry Ribbon**, 2 Plain Cards.

#### March (Cherry Blossom / 桜)
<p align="center">
  <img src="tools/cards_trad/08_Hanafuda%20March%20Hikari%20Alt.svg.png" width="90" alt="Mar Curtain">
  <img src="tools/cards_trad/09_Hanafuda%20March%20Tanzaku%20Alt.svg.png" width="90" alt="Mar Ribbon">
  <img src="tools/cards_trad/10_Hanafuda%20March%20Kasu%201%20Alt.svg.png" width="90" alt="Mar Plain 1">
  <img src="tools/cards_trad/11_Hanafuda%20March%20Kasu%202%20Alt.svg.png" width="90" alt="Mar Plain 2">
</p>
- **Curtain (Bright)**, **Red Poetry Ribbon**, 2 Plain Cards.

#### April (Wisteria / 藤)
<p align="center">
  <img src="tools/cards_trad/12_Hanafuda%20April%20Tane%20Alt.svg.png" width="90" alt="Apr Cuckoo">
  <img src="tools/cards_trad/13_Hanafuda%20April%20Tanzaku%20Alt.svg.png" width="90" alt="Apr Ribbon">
  <img src="tools/cards_trad/14_Hanafuda%20April%20Kasu%201%20Alt.svg.png" width="90" alt="Apr Plain 1">
  <img src="tools/cards_trad/15_Hanafuda%20April%20Kasu%202%20Alt.svg.png" width="90" alt="Apr Plain 2">
</p>
- **Cuckoo (Animal)**, **Red Plain Ribbon**, 2 Plain Cards.

#### May (Iris / 菖蒲)
<p align="center">
  <img src="tools/cards_trad/16_Hanafuda%20May%20Tane%20Alt.svg.png" width="90" alt="May Bridge">
  <img src="tools/cards_trad/17_Hanafuda%20May%20Tanzaku%20Alt.svg.png" width="90" alt="May Ribbon">
  <img src="tools/cards_trad/18_Hanafuda%20May%20Kasu%201%20Alt.svg.png" width="90" alt="May Plain 1">
  <img src="tools/cards_trad/19_Hanafuda%20May%20Kasu%202%20Alt.svg.png" width="90" alt="May Plain 2">
</p>
- **Eight-Plank Bridge (Animal)**, **Red Plain Ribbon**, 2 Plain Cards.

#### June (Peony / 牡丹)
<p align="center">
  <img src="tools/cards_trad/20_Hanafuda%20June%20Tane%20Alt.svg.png" width="90" alt="Jun Butterflies">
  <img src="tools/cards_trad/21_Hanafuda%20June%20Tanzaku%20Alt.svg.png" width="90" alt="Jun Ribbon">
  <img src="tools/cards_trad/22_Hanafuda%20June%20Kasu%201%20Alt.svg.png" width="90" alt="Jun Plain 1">
  <img src="tools/cards_trad/23_Hanafuda%20June%20Kasu%202%20Alt.svg.png" width="90" alt="Jun Plain 2">
</p>
- **Butterflies (Animal)**, **Blue Ribbon**, 2 Plain Cards.

#### July (Bush Clover / 萩)
<p align="center">
  <img src="tools/cards_trad/24_Hanafuda%20July%20Tane%20Alt.svg.png" width="90" alt="Jul Boar">
  <img src="tools/cards_trad/25_Hanafuda%20July%20Tanzaku%20Alt.svg.png" width="90" alt="Jul Ribbon">
  <img src="tools/cards_trad/26_Hanafuda%20July%20Kasu%201%20Alt.svg.png" width="90" alt="Jul Plain 1">
  <img src="tools/cards_trad/27_Hanafuda%20July%20Kasu%202%20Alt.svg.png" width="90" alt="Jul Plain 2">
</p>
- **Boar (Animal)**, **Red Plain Ribbon**, 2 Plain Cards.

#### August (Pampas Grass / 芒)
<p align="center">
  <img src="tools/cards_trad/28_Hanafuda%20August%20Hikari%20Alt.svg.png" width="90" alt="Aug Moon">
  <img src="tools/cards_trad/29_Hanafuda%20August%20Tane%20Alt.svg.png" width="90" alt="Aug Geese">
  <img src="tools/cards_trad/30_Hanafuda%20August%20Kasu%201%20Alt.svg.png" width="90" alt="Aug Plain 1">
  <img src="tools/cards_trad/31_Hanafuda%20August%20Kasu%202%20Alt.svg.png" width="90" alt="Aug Plain 2">
</p>
- **Full Moon (Bright)**, **Geese (Animal)**, 2 Plain Cards.

#### September (Chrysanthemum / 菊)
<p align="center">
  <img src="tools/cards_trad/32_Hanafuda%20September%20Tane%20Alt.svg.png" width="90" alt="Sep Sake Cup">
  <img src="tools/cards_trad/33_Hanafuda%20September%20Tanzaku%20Alt.svg.png" width="90" alt="Sep Ribbon">
  <img src="tools/cards_trad/34_Hanafuda%20September%20Kasu%201%20Alt.svg.png" width="90" alt="Sep Plain 1">
  <img src="tools/cards_trad/35_Hanafuda%20September%20Kasu%202%20Alt.svg.png" width="90" alt="Sep Plain 2">
</p>
- **Sake Cup (Animal/Plain)**, **Blue Ribbon**, 2 Plain Cards.

#### October (Maple / 楓)
<p align="center">
  <img src="tools/cards_trad/36_Hanafuda%20October%20Tane%20Alt.svg.png" width="90" alt="Oct Deer">
  <img src="tools/cards_trad/37_Hanafuda%20October%20Tanzaku%20Alt.svg.png" width="90" alt="Oct Ribbon">
  <img src="tools/cards_trad/38_Hanafuda%20October%20Kasu%201%20Alt.svg.png" width="90" alt="Oct Plain 1">
  <img src="tools/cards_trad/39_Hanafuda%20October%20Kasu%202%20Alt.svg.png" width="90" alt="Oct Plain 2">
</p>
- **Deer (Animal)**, **Blue Ribbon**, 2 Plain Cards.

#### November (Willow / 柳)
<p align="center">
  <img src="tools/cards_trad/40_Hanafuda%20November%20Hikari%20Alt.svg.png" width="90" alt="Nov Rain Man">
  <img src="tools/cards_trad/41_Hanafuda%20November%20Tane%20Alt.svg.png" width="90" alt="Nov Swallow">
  <img src="tools/cards_trad/42_Hanafuda%20November%20Tanzaku%20Alt.svg.png" width="90" alt="Nov Ribbon">
  <img src="tools/cards_trad/43_Hanafuda%20November%20Kasu%20Alt.svg.png" width="90" alt="Nov Double Plain">
</p>
- **Rain Man (Bright)**, **Swallow (Animal)**, **Red Plain Ribbon**, **Ssangpi (Double Plain)**.

#### December (Paulownia / 桐)
<p align="center">
  <img src="tools/cards_trad/44_Hanafuda%20December%20Hikari%20Alt.svg.png" width="90" alt="Dec Phoenix">
  <img src="tools/cards_trad/45_Hanafuda%20December%20Kasu%201%20Alt.svg.png" width="90" alt="Dec Plain 1">
  <img src="tools/cards_trad/46_Hanafuda%20December%20Kasu%202%20Alt.svg.png" width="90" alt="Dec Plain 2">
  <img src="tools/cards_trad/47_Hanafuda%20December%20Kasu%203%20Alt.svg.png" width="90" alt="Dec Double Plain">
</p>
- **Phoenix (Bright)**, 2 Plain Cards, **Ssangpi (Double Plain)**.

---

### Dealer Selection (*Mekuri* / Card Cut)

Before Round 1 begins:
1. Both players draw one random card from the deck.
2. The player drawing the card from the **earliest Month** (January = 1st Month ... December = 12th Month) becomes the first dealer (*Oya* in Japanese, *Sun* in Korean) and plays first.
3. If both players draw cards from the same Month, card tier breaks the tie (Bright > Animal > Ribbon > Plain).

For subsequent rounds:
- The **winner of the previous round becomes the dealer** for the next round.
- If a round ends in a tie/draw, the current dealer retains the deal.

---

## 2. Japanese Koi-Koi Rules & Yaku

In **Koi-Koi**, points are awarded based on traditional combination sets (*Yaku*):

### Lights (*Gokou*)
<p align="center">
  <img src="tools/cards_trad/00_Hanafuda%20January%20Hikari%20Alt.svg.png" width="80" alt="Crane">
  <img src="tools/cards_trad/08_Hanafuda%20March%20Hikari%20Alt.svg.png" width="80" alt="Curtain">
  <img src="tools/cards_trad/28_Hanafuda%20August%20Hikari%20Alt.svg.png" width="80" alt="Moon">
  <img src="tools/cards_trad/40_Hanafuda%20November%20Hikari%20Alt.svg.png" width="80" alt="Rain Man">
  <img src="tools/cards_trad/44_Hanafuda%20December%20Hikari%20Alt.svg.png" width="80" alt="Phoenix">
</p>

- **Five Lights (五光)** — 10 Points: All 5 Light cards (Crane, Curtain, Moon, Rain Man, Phoenix).
- **Four Lights (四光)** — 8 Points: Any 4 Light cards excluding Rain Man.
- **Rain Four Lights (雨四光)** — 7 Points: Any 4 Light cards including Rain Man.
- **Three Lights (三光)** — 5 Points: Any 3 Light cards excluding Rain Man.

### Ribbons (*Tanzaku*)
<p align="center">
  <img src="tools/cards_trad/01_Hanafuda%20January%20Tanzaku%20Alt.svg.png" width="80" alt="Jan Red Poetry">
  <img src="tools/cards_trad/05_Hanafuda%20February%20Tanzaku%20Alt.svg.png" width="80" alt="Feb Red Poetry">
  <img src="tools/cards_trad/09_Hanafuda%20March%20Tanzaku%20Alt.svg.png" width="80" alt="Mar Red Poetry">
  <img src="tools/cards_trad/21_Hanafuda%20June%20Tanzaku%20Alt.svg.png" width="80" alt="Jun Blue">
  <img src="tools/cards_trad/33_Hanafuda%20September%20Tanzaku%20Alt.svg.png" width="80" alt="Sep Blue">
  <img src="tools/cards_trad/37_Hanafuda%20October%20Tanzaku%20Alt.svg.png" width="80" alt="Oct Blue">
</p>

- **Red Poetry Ribbons (赤短 - Akatan)** — 5 Points: All 3 red poetry ribbons (Jan, Feb, Mar) + 1 pt per extra ribbon.
- **Blue Ribbons (青短 - Aotan)** — 5 Points: All 3 blue ribbons (Jun, Sep, Oct) + 1 pt per extra ribbon.
- **Plain Ribbons (短冊 - Tanzaku)** — 1 Point: Any 5 ribbons + 1 pt per extra ribbon.

### Animals (*Tane*)
<p align="center">
  <img src="tools/cards_trad/24_Hanafuda%20July%20Tane%20Alt.svg.png" width="80" alt="Jul Boar">
  <img src="tools/cards_trad/36_Hanafuda%20October%20Tane%20Alt.svg.png" width="80" alt="Oct Deer">
  <img src="tools/cards_trad/20_Hanafuda%20June%20Tane%20Alt.svg.png" width="80" alt="Jun Butterflies">
</p>

- **Ino-Shika-Chou (猪鹿蝶)** — 5 Points: Boar (Jul), Deer (Oct), and Butterflies (Jun) + 1 pt per extra animal.
- **Animals (タネ - Tane)** — 1 Point: Any 5 animal cards + 1 pt per extra animal.

---

## 3. Korean Go-Stop Rules & Penalties

In **Go-Stop**, players must reach a minimum threshold of **3 points** before calling **Go** or **Stop**.

### 5 Birds (Godori / 고도리)
<p align="center">
  <img src="tools/cards_trad/04_Hanafuda%20February%20Tane%20Alt.svg.png" width="80" alt="Feb Bush Warbler">
  <img src="tools/cards_trad/12_Hanafuda%20April%20Tane%20Alt.svg.png" width="80" alt="Apr Cuckoo">
  <img src="tools/cards_trad/29_Hanafuda%20August%20Tane%20Alt.svg.png" width="80" alt="Aug Geese">
</p>

- **Godori (5 Birds)** — 5 Points: February Bush Warbler, April Cuckoo, and August Geese.

### Korean Ribbon Sets
<p align="center">
  <img src="tools/cards_trad/13_Hanafuda%20April%20Tanzaku%20Alt.svg.png" width="80" alt="Apr Grass">
  <img src="tools/cards_trad/17_Hanafuda%20May%20Tanzaku%20Alt.svg.png" width="80" alt="May Grass">
  <img src="tools/cards_trad/25_Hanafuda%20July%20Tanzaku%20Alt.svg.png" width="80" alt="Jul Grass">
</p>

- **Hongdan (홍단)** — 3 Points: 3 Red Poetry Ribbons (Jan, Feb, Mar).
- **Cheongdan (청단)** — 3 Points: 3 Blue Ribbons (Jun, Sep, Oct).
- **Chodan (초단)** — 3 Points: 3 Plain Grass Ribbons (Apr, May, Jul).

### Multipliers & Penalties (*Bak*)
- **Go Multipliers**: Calling 1 Go (+1 pt), 2 Go (+2 pts), 3 Go (2x total score), 4 Go (4x total score), 5 Go (8x total score).
- **Gwang-bak (광박 - Light Penalty)**: If winner scores with Light cards and opponent has 0 Lights, score doubles (2x).
- **Pi-bak (피박 - Junk Penalty)**: If winner scores with Plain cards and opponent has 5 or fewer Plain cards, score doubles (2x).
