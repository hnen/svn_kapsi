
function vec3( x, y, z )
{
	this.x = x ? x : 0;
	this.y = y ? y : 0;
	this.z = z ? z : 0;
}

vec3.prototype = {
	add : function( v )
	{
		return new vec3( this.x + v.x, this.y + v.y, this.z + v.z );
	},
	length : function() {
		return Math.sqrt(this.x*this.x + this.y*this.y + this.z*this.z);
	},
	scaled : function( s )
	{
		return new vec3( this.x*s, this.y*s, this.z*s );
	},
	dot : function( v ) {
		return this.x*v.x + this.y*v.y + this.z*v.z;
	},
	add : function( v ) 
	{
		return new vec3( this.x+v.x, this.y+v.y, this.z+v.z );
	},
	sub : function( v ) 
	{
		return new vec3( this.x-v.x, this.y-v.y, this.z-v.z );
	},
	normalized : function() {
		return this.scaled( 1.0/this.length() );
	},
	normalize : function() {
		var v = 1.0 / this.length();
		this.x *= v;
		this.y *= v;
		this.z *= v;
	},
	cross : function(v) {
		return new vec3( this.y*v.z - this.z*v.y,
                        this.z*v.x - this.x*v.z,
                        this.x*v.y - this.y*v.x);
	}
}


vec3.upvector = function( up, zvec ) {
	var x = up.cross(zvec); 
	var y = zvec.cross(x); 
	return y.normalized();
}

