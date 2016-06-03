/**
 * Projekt do predmetu Grafove algoritmy -- GAL (Generovani bludist)
 * Autor: Vojtech Havlena <xhavle03@stud.fit.vutbr.cz>
 * Datum: 10. 11. 2015
 */

/**
 * Objekt pro generovani bludiste.
 * @param n Velikost bludiste.
 */
function MazeGenerator(n)
{
   this.n = n;
   this.adj = [];
   this.universe = [];
   this.pi = [];
   this.exits = [];
   
   this.maxPartWidth;
   this.startPart;
   this.T = 4;
   
   for(i = 0; i < n*n; i++)
   {
      this.adj[i] = [];
   }
   for(i = 0; i < n*n; i++)
   {
      this.universe.push(i);
   }
   this.maxPartWidth = Math.floor(3.0/4.0 * this.n);
   this.startPart = Math.floor(this.n / 3.0);   
      
   this.findNeighbours = function (i, n, set)
   {
      var choose = [];
      if(i % n != 0 && set.indexOf(i - 1) != -1 && this.universe.indexOf(i-1) != -1)
      {
         choose.push(i - 1);
      }
      if((i + 1) % n != 0 && set.indexOf(i + 1) != -1 && this.universe.indexOf(i+1) != -1)
      {
         choose.push(i + 1);
      }
      if((i - n) >= 0 && set.indexOf(i - n) != -1 && this.universe.indexOf(i-n) != -1)
      {
         choose.push(i - n);
      }
      if((i + n) < n*n && set.indexOf(i + n) != -1 && this.universe.indexOf(i+n) != -1)
      {
         choose.push(i + n);
      }
      
      return choose;
   }
   
   this.findNeighboursBelow = function (i, n, set)
   {
      var choose = [];
      if(i % n != 0 && set.indexOf(i - 1) != -1 && this.universe.indexOf(i-1) != -1)
      {
         choose.push(i - 1);
      }
      if((i + 1) % n != 0 && set.indexOf(i + 1) != -1 && this.universe.indexOf(i+1) != -1)
      {
         choose.push(i + 1);
      }
      if((i + n) < n*n && set.indexOf(i + n) != -1 && this.universe.indexOf(i+n) != -1)
      {
         choose.push(i + n);
      }
      
      return choose;
   }
   
   this.moveFromSet = function (set, from, to)
   {
      for(i = 0; i < set.length; i++)
      {
         index = from.indexOf(set[i]);
         if(index >-1)
         {
            from.splice(index, 1);
            to.push(set[i]);
         }
      }
   }
   
   this.addCells = function(cell, pathSet)
   {
      var i = 0;
      var r;
      var sum = 0;
      if(cell % this.n != 0 && pathSet.indexOf(cell - 1) == -1)
      {
         for(i = cell - 1; i >= cell - this.startPart; i--)
         {
            sum++;
            if(this.pi.indexOf(i) == -1)
            {
               this.pi.push(i);
            }
            
            if(i % n == 0)
            {
               i--;
               break;
            }
         }
         
         while(i >= 0)
         {
            if((i + 1) % n == 0)
               break;
            r = Math.random();
            if(r < Math.pow(Math.E, -(sum / this.T)))
            {
               sum++;
               this.pi.push(i);
            }
            else
            {
               break;
            }
            i--;
         }
      }
      
      sum = 0;
      if((cell + 1) % n != 0 && pathSet.indexOf(cell + 1) == -1)
      {
         for(i = cell + 1; i <= cell + this.startPart && i % n != 0; i++)
         {
            sum++;
            if(this.pi.indexOf(i) == -1)
            {
               this.pi.push(i);
            }
         }
         
         while(i % n != 0)
         {
            r = Math.random();
            if(r < Math.pow(Math.E, -(sum / this.T)))
            {
               sum++;
               this.pi.push(i);
            }
            else
            {
               break;
            }
            i++;
         }
      }
   }
   
   /**
    * Generovani hlavni komponenty simulovanym zihanim.
    */
   this.simulatedAnnealing = function(pathSet)
   {
      var first;
      
      for(i = 0; i < pathSet.length; i++)
      {
         first = pathSet[i];
         this.addCells(first, pathSet);
      }
   }
   
   /**
    * Hledani cesty upravenym backtrackingem.
    */
   this.backtrack = function(act, end, universe)
   {
      if(act == end)
         return true;
      
      var neighbourNodes = this.findNeighboursBelow(act, n, universe);
      if(neighbourNodes.length == 0)
         return false;
      
      var randIn = Math.floor(Math.random() * neighbourNodes.length);
      var i = randIn;
      var endIter = randIn - 1;
      if(endIter == -1)
         endIter = neighbourNodes.length - 1;
      
      var can = true;
      while(i != endIter || can)
      {
         can = false;
         
         if(this.pi.indexOf(neighbourNodes[i]) == -1)
         {
            this.pi.push(neighbourNodes[i]);
            if(this.backtrack(neighbourNodes[i], end, universe))
            {
               return true;
            }
            
            var index = this.pi.indexOf(neighbourNodes[i]);
            if(index >-1)
            {
               this.pi.splice(index, 1);
            }
         }
         
         i = (i + 1) % neighbourNodes.length
      }
      
      return false;
   }
   
   this.divideMaze = function(start, end)
   {
      this.pi = [start];
      this.backtrack(start, end, this.universe);
      
      this.exits.push(start);
      this.exits.push(end);
      
      this.simul();
   }
   
   this.findArea = function(start, used)
   {
      var area = [];
      var stack = [start];
      var tp;
      var exs = [];
      
      while(stack.length > 0)
      {
         tp = stack.pop();
         used.push(tp);
         area.push(tp);
         
         if(tp % this.n == 0 || (tp + 1) % this.n == 0 || tp < this.n || tp > this.n*(this.n - 1))
            exs.push(tp);
         
         if(tp % n != 0 && used.indexOf(tp - 1) == -1)
         {
            stack.push(tp - 1);
         }
         if((tp + 1) % n != 0 && used.indexOf(tp + 1) == -1)
         {
            stack.push(tp + 1);
         }
         if((tp - this.n) >= 0 && used.indexOf(tp - this.n) == -1)
         {
            stack.push(tp - this.n);
         }
         if((tp + this.n) < this.n*this.n && used.indexOf(tp + this.n) == -1)
         {
            stack.push(tp + this.n);
         }
      }
      
      var adding;
      if(area.length > ((this.n*this.n) / 6.0))
      {
         adding = exs[Math.floor(Math.random() * exs.length)]
         this.exits.push(adding);
      }
      
      return area;
   }
   
   this.simul = function()
   {
      this.simulatedAnnealing(this.pi.slice());
      
      this.universe = this.pi.slice();
      
      this.generateMaze();
      
      var used = this.universe.slice();
      for(i = 0; i < n*n; i++)
      {
         if(used.indexOf(i) != -1)
            continue;
         
         var ar = this.findArea(i, used);
         this.universe = ar.slice();
         this.generateMaze();
      }
   }
   
   this.generateMaze = function()
   {
      var boundary = [];
      var outside = [];
      var inside = [];
   
      for(i = 0; i < n*n; i++)
      {
         outside.push(i);
      }
      
      var randIn = 0;
      var neighbourNodes;
      var neighbour;
      var cell = this.universe[Math.floor(Math.random() * this.universe.length)];
      var index = outside.indexOf(cell);
      
      if(index >-1)
      {
         outside.splice(index, 1);
      }
      inside.push(cell);
      neighbourNodes = this.findNeighbours(cell, n, outside);
      this.moveFromSet(neighbourNodes, outside, boundary);
      
      while(boundary.length > 0)
      {
         randIn = Math.floor(Math.random() * boundary.length);
         cell = boundary[randIn];
         inside.push(cell);
         
         index = boundary.indexOf(cell);
         if(index >-1)
         {
            boundary.splice(index, 1);
         }
      
         neighbourNodes = this.findNeighbours(cell, n, inside);
         if(neighbourNodes.length > 0)
         {
            neighbour = neighbourNodes[Math.floor(Math.random() * neighbourNodes.length)];
            
            this.adj[cell].push(neighbour);
            this.adj[neighbour].push(cell);
         }
         
         neighbourNodes = this.findNeighbours(cell, n, outside);
         this.moveFromSet(neighbourNodes, outside, boundary);
         
      }
   }
}
