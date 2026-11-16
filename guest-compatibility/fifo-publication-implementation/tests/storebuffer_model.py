#!/usr/bin/env python3
"""Small exhaustive abstract producer/consumer protocol model, no guest/binary access.

The model deliberately represents the buffered guest store and host recheck; it is
not a CPU ISA formal proof nor an executable copy of Apple's driver.
"""
import collections
import json

def model(exchange):
 initial=(0,0,4,0,False,-1,-1)
 queue=collections.deque([(initial,[]) ]);seen={initial};lost=[];finals=set()
 while queue:
  state,path=queue.popleft();g,h,w,r,buf,gs,hs=state
  if g==2 and h==3:
   finals.add((gs,hs))
   if gs!=4 and hs==4:lost.append(path)
  choices=[]
  if g==0:choices.append(((1,h,8 if exchange else w,r,not exchange,gs,hs),'guest publish '+('ordered exchange' if exchange else 'buffered MOV')))
  if g==1:choices.append(((2,h,w,r,buf,r,hs),'guest reads host read='+str(r)))
  if h==0:choices.append(((g,1,w,4,buf,gs,hs),'host publishes read=4 with XCHG'))
  if h==1:choices.append(((g,2,w,r,buf,gs,w),'host first written='+str(w)))
  if h==2:choices.append(((g,3,w,r,buf,gs,w if hs==4 else hs),'host protected second written='+str(w)))
  if buf:choices.append(((g,h,8,r,False,gs,hs),'guest buffered written becomes visible'))
  for nxt,event in choices:
   if nxt not in seen:seen.add(nxt);queue.append((nxt,path+[event]))
 return {'stateCount':len(seen),'outcomes':sorted([list(x) for x in finals]),'lostWitnessCount':len(lost),'lostWitness':lost[0] if lost else None}

if __name__ == "__main__":
    original, ordered = model(False), model(True)
    assert original["lostWitnessCount"] > 0, original
    assert ordered["lostWitnessCount"] == 0, ordered
    print(json.dumps({"unfenced": original, "ordered": ordered}, indent=2))
