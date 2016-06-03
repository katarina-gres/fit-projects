/**
 * Projekt do predmetu Grafove algoritmy -- GAL (Generovani bludist)
 * Autor: Vojtech Havlena <xhavle03@stud.fit.vutbr.cz>
 * Datum: 10. 11. 2015
 */

/**
 * Objekt pro vykreslovani bludiste.
 * @param n Velikost bludiste.
 * @param context Context pro vykreslovani.
 * @param adj Popis bludiste ve forme matice.
 * @param exs Seznam vychodu.
 */
function MazeDrawer(n, context, adj, exs)
{
   this.context = context;
   this.n = n;
   this.adj = adj;
   var blockSize = Math.floor(500/n);
   this.exits = exs;
   
   this.context.fillStyle="#00A1FF";
   
   /**
    * Funkce pro vykresleni vychodu.
    */
   this.drawExits = function()
   {
      var x;
      var y;
      for(i = 0; i < this.exits.length; i++)
      {
         y = Math.floor(this.exits[i] / this.n);
         x = this.exits[i] % this.n;
         
         this.context.fillRect(x*blockSize + 10 + blockSize/4, y*blockSize + 10 + blockSize/4, blockSize/2,blockSize/2);
      }
   }
   
   /**
    * Funkce pro vykresleni celeho bludiste.
    */
   this.drawMaze = function()
   {
      this.context.clearRect(0, 0, 700, 700);

      this.context.beginPath();
      this.context.moveTo(10,10);
      
      var actX = 10;
      var actY = 10;
      var node;
      
      for(i = 0; i < this.n; i++)
      {
         for(j = 0; j < this.n; j++)
         {
            node = i*this.n + j;
            if(this.adj[node] == null || this.adj[node].indexOf(node - this.n) == -1)
            {
               this.context.moveTo(actX, actY);
               this.context.lineTo(actX + blockSize, actY);
            }
            if(this.adj[node] == null || this.adj[node].indexOf(node - 1) == -1)
            {
               this.context.moveTo(actX, actY);
               this.context.lineTo(actX, actY + blockSize);
            }
            actX += blockSize;
         }
         actX = 10;
         actY += blockSize;
      }
      
      this.drawExits();
      
      context.moveTo(10, this.n * blockSize + 10);
      context.lineTo(this.n * blockSize + 10, this.n * blockSize + 10);
      context.lineTo(this.n * blockSize + 10, 10);
      context.lineJoin = 'round';
      context.stroke();
   }
}
