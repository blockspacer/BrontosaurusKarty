using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Remoting.Lifetime;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json.Linq;

namespace ParticleEditor
{
    class ParticleSystem
    {
        public string Name { get; set; }
        private Guid id;

        public ParticleSystem()
        {
            Name = "Unnamed";
            id = Guid.NewGuid();
        }

        public int Id
        {
            get { return id.GetHashCode(); }
        }

        public int EmissionRate { get; set; }
        public int MaxNumberOfParticles { get; set; }
        public bool Loop { get; set; }
        public float Lifetime { get; set; }
        public int MaxEmitters { get; set; }
        public bool Persistant { get; set; }
        public void Deserialize(JToken jToken)
        {
            Name = Convert.ToString(jToken["name"]);
            if (jToken["guid"] != null)
            {
                id = Guid.Parse(Convert.ToString(jToken["guid"]));
            }
            else
            {
                id = Guid.NewGuid();
            }

            EmissionRate = Convert.ToInt32(jToken["emissionRate"]);

            MaxNumberOfParticles = Convert.ToInt32(jToken["maxNumOfParticles"]);

            Loop = Convert.ToBoolean(jToken["loop"]);

            Lifetime = (float) Convert.ToDecimal(jToken["lifetime"]);

            Persistant = Convert.ToBoolean(jToken["pertistant"]);
            MaxEmitters = Convert.ToInt32(jToken["maxEmitters"]);
        }
    }
}
