from argparse import ArgumentParser as ap
import numpy as np

def append_lines(lines, apa_channels, apa_slots, apa_number):
  # The offline channel numbers (map values) are defined according to their 3D position
  # so translate the APA number to the offline channel number accordingly
  apa_starts = {2:80, 1:120, 4:0, 3:40}
  link = 0
  for pos,daphne_channels in apa_channels.items():
    start = apa_starts[apa_number]
    slot = apa_slots[pos]
    ##Reverse every other position on specific apas
    if (apa_number in [1, 2]) and ((pos % 2) != 0): ##Beam right side 
      daphne_channels.reverse()
    elif (apa_number in [3, 4]) and ((pos % 2) == 0): ##Beam left side
      daphne_channels.reverse()

    a = 0
    for dc in daphne_channels:
      offline_number = start + 10*(a) + (pos - 1)
      lines.append(f'{slot} {link} {dc} {offline_number}\n')
      a += 1


def make_v1_map(args):
  lines = []

  '''See https://docs.google.com/spreadsheets/d/1zzlUnk-Bd3HPtZjVei6dwMzZfJfmq4GauykcVqXREKw/edit?usp=sharing
     for the mapping between 'APA position' to link, slot, and channel'''

  apa1_channels = {
    1:[7, 5, 2, 0],
    2:[6, 4, 3, 1],
    3:[17, 15, 12, 10],
    4:[16, 14, 13, 11],
    5:[7, 5, 2, 0],
    6:[6, 4, 3, 1],
    7:[26, 24, 23, 21],
    8:[17, 15, 12, 10],
    9:[17, 15, 12, 10], 
    10:[7, 5, 2, 0], ##
  }

  apa1_slots = {
    1:4, 2:4, 3:4, 4:4,
    5:5, 6:5, 7:5, 8:5,
    9:7, 10:7,
  }
  ## Runs >= 28035 have DAPHNE board 7 swapped with board 10
  ## Call this v2 and switch in the map above
  if args.v > 1:
    apa1_slots[9] = 10
    apa1_slots[10] = 10

  apa2_channels = {
    1:[27, 25, 22, 20],
    2:[26, 24, 23, 21],
    3:[37, 35, 42, 30],
    4:[36, 34, 33, 31],
    5:[7, 5, 2, 0],
    6:[6, 4, 3, 1],
    7:[17, 15, 12, 10],
    8:[16, 14, 13, 11],
    9:[47, 45, 52, 40],
    10:[46, 44, 43, 41],
  }
  apa2_slots = {i:9 for i in range(1, 11)} 

  apa3_channels = {
    1:[6, 4, 3, 1],
    2:[36, 34, 33, 31],
    3:[7, 5, 2, 0],
    4:[37, 35, 32, 30],
    5:[46, 44, 43, 41],
    6:[16, 14, 13, 11],
    7:[17, 15, 12, 10],
    8:[26, 24, 23, 21],
    9:[47, 45, 42, 40],
    10:[27, 25, 22, 20],
  }
  apa3_slots = {i:11 for i in range(1, 11)} 

  apa4_channels = {
    1:[7, 5, 2, 0],
    2:[6, 4, 3, 1],
    3:[17, 15, 12, 10],
    4:[16, 14, 13, 11],
    5:[7, 5, 2, 0],
    6:[27, 25, 22, 20],
    7:[26, 24, 23, 21],
    8:[37, 35, 32, 30],
    9:[36, 34, 33, 31],
    10:[47, 45, 42, 40],
  }
  apa4_slots = {i:12 for i in range(1, 11)} 
  apa4_slots[5] = 13

  ##TODO -- CHECK THIS DESCRIPTION
  ## The 'Daphne' channels above are in order of the 'module channels' (1, 2, 3, 4)
  ## So APA 1, Position 1 has module channels (1, 2, 3, 4) == daphne channels (7, 4, 2, 0)
  ##
  ## We need to translate the module channels to physical position. Position 1 is the highest 
  ## module on the APA.
  ## The next statement is relative to a view from within an APA's inner drift volume:
  ## The odd positions (1, 3, 5, 7, 9) are in reversed order so for position 1,
  ## the module channels are (from left-to-right) 4, 3, 2, 1 while the even positions
  ## (i.e. position 2) are 1, 2, 3, 4.
  ##
  ## For the beam-right/Saleve side APAs (1 & 2) 'Left' then means 'more downstream'
  ## while the beam-left/Jura side APAs (3 & 4)  'Left' means 'more upstream'.

  ## The APAs are situated as follows in the volume looking down in Y
  '''   | APA 3 | APA 4 |
        |----Cathode----|  Upstream (Lower Z) -----> Downstream (Higher Z)
        | APA 1 | APA 2 |'''

  link = 0
  append_lines(lines, apa1_channels, apa1_slots, 1)
  append_lines(lines, apa2_channels, apa2_slots, 2)
  append_lines(lines, apa3_channels, apa3_slots, 3)
  append_lines(lines, apa4_channels, apa4_slots, 4)
  #for pos,daphne_channels in apa1_channels.items():
  #  start = 80
  #  slot = apa1_slots[pos]
  #  if (pos % 2) != 0:
  #    daphne_channels.reverse()

  #  a = 0
  #  for dc in daphne_channels:
  #    offline_number = start + 10*(a) + (pos - 1)
  #    lines.append(f'{slot} {link} {dc} {offline_number}\n')
  #    a += 1


  with open(args.o, 'w') as f:
    f.writelines(lines)

def make_v0_map(args):
  lines = []
   
  offline_number = 0

  link_chans = {
    0:np.arange(0, 4),
    1:np.arange(4, 8),
    2:np.arange(10, 14),
    3:np.arange(14, 18),
  }

  for slot in [4, 5]:
    for link, chans in link_chans.items():
      for c in chans: 
        lines.append(f'{slot} {link} {c} {offline_number}\n')
        offline_number += 1

  slot = 7
  for link in range(2):
    for c in link_chans[link]: 
      lines.append(f'{slot} {link} {c} {offline_number}\n')
      offline_number += 1

  link = 0
  for slot in [9, 11]:
    for c in range(40):
      lines.append(f'{slot} {link} {c} {offline_number}\n')
      offline_number += 1

  slot = 13
  for c in range(0, 16):
    lines.append(f'{slot} {link} {c} {offline_number}\n')
    offline_number += 1

  slot = 12
  for c in range(0, 4):
    lines.append(f'{slot} {link} {c} {offline_number}\n')
    offline_number += 1

  slot = 13
  for c in range(20, 40):
    lines.append(f'{slot} {link} {c} {offline_number}\n')
    offline_number += 1

  with open(args.o, 'a') as f:
    f.writelines(lines)

if __name__ == '__main__':
  parser = ap() 
  parser.add_argument('-o', required=True)
  parser.add_argument('-v', default=0, type=int)
  args = parser.parse_args()

  makers = {
   0:make_v0_map, 
   1:make_v1_map,
  }

  if args.v not in makers.keys():
    exit() #TODO -- print something

  makers[args.v](args)
