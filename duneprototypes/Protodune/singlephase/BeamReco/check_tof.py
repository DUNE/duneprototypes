import ROOT as RT
from argparse import ArgumentParser as ap
import numpy as np

def do_tof(i, gs, gc, gf, h, secs,coarses,fracs):
  for j in range(len(secs)):
    s = secs[j]
    c = coarses[j]
    f = fracs[j]

    print(i, j, -1*(s - gs + 1.e-9*(8.*(c - gc) + (f - gf)/512.)))
    h.Fill(
      gs - s + 1.e-9*(8.*(gc - c) + (gf - f)/512.)
    )

if __name__ == '__main__':
  parser = ap()
  parser.add_argument('-i', type=str)
  parser.add_argument('-o', type=str, default='tofs.root')
  args = parser.parse_args()

  f = RT.TFile.Open(args.i)
  t = f.Get('beamevent/tree')

  fout = RT.TFile(args.o, 'recreate')
  h1A = RT.TH1D('h1A', '', 10000000, -.01, .01)
  h1B = RT.TH1D('h1B', '', 10000000, -.01, .01)
  h2A = RT.TH1D('h2A', '', 10000000, -.01, .01)
  h2B = RT.TH1D('h2B', '', 10000000, -.01, .01)

  all_1A_diffs = []
  a = 0
  for e in t:
    gen_secs = np.array(t.genTrigSecs)
    br1A_secs = np.array(t.br1ASecs)
    br2A_secs = np.array(t.br2ASecs)
    br1B_secs = np.array(t.br1BSecs)
    br2B_secs = np.array(t.br2BSecs)

    gen_fracs = np.array(t.genTrigFracs)
    br1A_fracs = np.array(t.br1AFracs)
    br2A_fracs = np.array(t.br2AFracs)
    br1B_fracs = np.array(t.br1BFracs)
    br2B_fracs = np.array(t.br2BFracs)

    gen_coarses = np.array(t.genTrigCoarses)
    br1A_coarses = np.array(t.br1ACoarses)
    br2A_coarses = np.array(t.br2ACoarses)
    br1B_coarses = np.array(t.br1BCoarses)
    br2B_coarses = np.array(t.br2BCoarses)


    for i in range(len(gen_secs)):
      gs = gen_secs[i]
      gf = gen_fracs[i]
      gc = gen_coarses[i]

      do_tof(i, gs, gc, gf, h2A, br2A_secs, br2A_coarses, br2A_fracs)
      #for j in range(len(br1A_secs)):
      #  s = br1A_secs[j]
      #  c = br1A_coarses[j]
      #  fr = br1A_fracs[j]

      #  #all_1A_diffs.append(
      #  h1A.Fill(
      #    s - gs + 1.e-9*(8.*(c - gc) + (fr - gf)/512.)
      #  )


    break
    a += 1

  fout.cd()
  h2A.Write()
  fout.Close()
  f.Close()
