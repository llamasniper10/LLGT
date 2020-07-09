# {g_i} = {g1,g2,g3,g4}
#	This is the set of gene expression levels that a given cell experiences.
#	This is analogous to the field strength that each sensor feels when the float is in a given location

# C_{ij}(x)
#	This is a term (ith row, jth column) from the covariance matrix of the 4 genes at the position x. It expresses the covariance between profiles g_i and g_j at x.
#	This is analogous to the covariance between each of the various activation profiles

# \hat{C}(x)
#	This is the covariance matrix composed of the terms C_{ij}(x), seen above. Note that \hat{C} is a list of matrices indexed by position.

# \bar{g}_i(x)
#	This is the average expression of g_i at the position x.
#	This is analogous to the average field strength that each sensor feels when the float is in a given location (x)

# P({g_i}|x) = \frac{\exp{-\chi_K^2({g_i},x)/2}}{\sqrt{(2\pi)^K\det[\hat{C}(x)]}}
#	This is the probability of having a particular set of gene expression levels ({g_i}) given that the cell is at a given position (x).
#	This is analogous to the probability of having a particular set of field strength readings from the sensor given that the float is at a given position.

# \chi_K^2({g_i},x)=\sum_{i,j=1}^K (g_i-\bar{g}_i(x))(\hat{C}^{-1}(x))_{ij}(g_j-\bar{g}_j(x))
#	This measures the similarity of the gene expression pattern to the mean pattern {\bar{g}_i(x)} expected at x

# P(xStar | {g_i}) = \frac{1}{Z({g_i})}P({g_i}|xStar)P_X(xStar)
#	This is the likelihood that a cell is at implied position xStar given that it experiences the set of gene expression levels {g_i}. Z is a normalizing factor; it speaks to the absolute probability of the particular set {g_i} occurring (Z is independent of xStar).
#	This is analogous to the likelihood that a the float is at implied position xStar given that a set of field strength readings. For our purposes I suspect Z can be calculated by finding the area under the posterior before normalizing (i.e. the posterior can simply be normalized).

# If the posterior has a single, reasonably sharp peak at a single point, then we can translate expression levels back into positions unambiguously. The width of the distribution P(xStar | {g_i}) around its peak quantifies the positional error (i.e. the uncertainty in implied position due to the variability in gap gene expression levels.

# But if the posterior has multiple peaks, or broad plateaus, then genuine ambiguities in decoding exist, and the MAP decoder is misleading. In this case for the LLG, we err towards the point y closest to EMPTY, for y in the set of points with probability within some probability margin of the greatest probability density present in the posterior.

# {g_i^\alpha(x)}
#	This is the set of measured expression levels in an embryo at actual position x.

# P_{map}^\alpha(xStar|x) = P(xStar|{g_i}) evaluated for {g_i}={g_i^\alpha(x)}
#	If the considered genes provide enough information to specidfy position accurately and unambiguously, then P_{map}^\alpha(xStar|x) will be a narror ridge of density along the diagonal where the implied position is equal to the actual position, xStar = x.


